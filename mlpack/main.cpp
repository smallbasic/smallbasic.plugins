// This file is part of SmallBASIC
//
// Plugin for mlpack library - http://mlpack.org
//
// This program is distributed under the terms of the GPL v2.0 or later
// Download the GNU Public License (GPL) from www.gnu.org
//
// Copyright(C) 2023 Chris Warren-Smith

#include "config.h"

#include <sys/stat.h>
#include <cstring>
#include <cstdlib>
#include <cstdio>

#include "include/param.h"
#include "robin-hood-hashing/src/include/robin_hood.h"
#include <mlpack/methods/kmeans/kmeans.hpp>
#include <mlpack/src/mlpack/methods/ann.hpp>

//
// References:
// https://arma.sourceforge.net/docs.html
// https://training.galaxyproject.org/training-material/topics/statistics/tutorials/FNN/tutorial.html
//
// NegativeLogLikelihood is the output layer that is used for classification problem.
// RandomInitialization means that initial weights in neurons are generated randomly in the interval from -1 to 1.
// For regression problems, we use Mean Squared Error (MSE) loss function, which averages the square of the
// difference between predicted and actual values for the batch.

using namespace mlpack;

typedef FFN<MeanSquaredError, GaussianInitialization> FFN_MSE;
typedef FFN<NegativeLogLikelihood, RandomInitialization> FFN_NLL;
typedef FFN<ReconstructionLoss, HeInitialization> FFN_RL;

robin_hood::unordered_map<int, arma::mat *> _dataMap;
robin_hood::unordered_map<int, FFN_MSE *> _ffnMseMap;
robin_hood::unordered_map<int, FFN_NLL *> _ffnNllMap;
robin_hood::unordered_map<int, FFN_RL *> _ffnRlMap;
int _nextId = 1;

#define CLS_DATA     1
#define CLS_FFN_MSE  2
#define CLS_FFN_NLL  3
#define CLS_FFN_RL   4

static void v_setmat(var_t *var, arma::mat *mat) {
  int id = ++_nextId;
  _dataMap[id] = mat;
  map_init_id(var, id, CLS_DATA);
  v_setint(map_add_var(var, "n_rows", 0), mat->n_rows);
  v_setint(map_add_var(var, "n_cols", 0), mat->n_cols);
}

static int get_data_id(int argc, slib_par_t *params, int arg, var_t *retval) {
  int result = -1;
  if (is_param_map(argc, params, arg)) {
    int id = get_id(params, arg);
    if (id != -1 && _dataMap.find(id) != _dataMap.end()) {
      result = id;
    } else {
      error(retval, "Data ID not found");
    }
  } else {
    error(retval, "Map type not found");
  }
  return result;
}

//
// load from CSV
//
static bool get_mat(arma::mat &dataset, const char *str) {
  return data::Load(str, dataset, false);
}

//
// load dataset from SB array
//
static bool get_mat(arma::mat &dataset, var_t *var) {
  bool result;
  if (v_maxdim(var) == 1) {
    // array of numbers or arrays
    size_t rows = v_asize(var);
    size_t cols = 1;
    for (size_t r = 0; r < rows; r++) {
      var_t *elem = v_elem(var, r);
      if (v_is_type(elem, V_ARRAY)) {
        size_t num_elements = v_asize(elem);
        if (num_elements > cols) {
          cols = num_elements;
        }
      }
    }
    dataset.zeros(rows, cols);
    for (size_t r = 0; r < rows; r++) {
      var_t *elem = v_elem(var, r);
      if (v_is_type(elem, V_ARRAY)) {
        for (size_t c = 0; c < v_asize(elem); c++) {
          dataset(r, c) = get_num(v_elem(elem, c));
        }
      } else {
        dataset(r, 0) = get_num(elem);
      }
    }
    result = true;
  } else if (v_maxdim(var) == 2) {
    // 2d array
    size_t rows = abs(v_ubound(var, 0) - v_lbound(var, 0)) + 1;
    size_t cols = abs(v_ubound(var, 1) - v_lbound(var, 1)) + 1;
    dataset.zeros(rows, cols);
    for (size_t r = 0; r < rows; r++) {
      for (size_t c = 0; c < cols; c++) {
        size_t pos = r * cols + c;
        dataset(r, c) = get_num(v_elem(var, pos));
      }
    }
    result = true;
  } else {
    result = false;
  }
  return result;
}

static void neighbor_search(arma::mat &dataset, var_t *retval) {
  // dataset.raw_print();
  NeighborSearch<NearestNeighborSort, ManhattanDistance> nn(dataset);
  arma::Mat<size_t> neighbors;
  arma::mat distances;

  nn.Search(1, neighbors, distances);

  map_init(retval);
  var_t *v_neighbors = map_add_var(retval, "neighbors", 0);
  var_t *v_distances = map_add_var(retval, "distances", 0);
  v_toarray1(v_neighbors, neighbors.n_elem);
  v_toarray1(v_distances, neighbors.n_elem);
  for (size_t c = 0; c < neighbors.n_elem; c++) {
    v_setreal(v_elem(v_neighbors, c), neighbors[c]);
    v_setreal(v_elem(v_distances, c), distances[c]);
  }
}

static void kmeans_cluster(arma::mat &dataset, size_t clusters, size_t maxIterations, var_t *retval) {
  // Create the k-means object and set the parameters.
  KMeans<> k(maxIterations);

  // The assignments will be stored in this vector.
  arma::Row<size_t> assignments;

  // The centroids will be stored in this matrix.
  arma::mat centroids;

  k.Cluster(dataset, clusters, assignments, centroids);

  map_init(retval);
  var_t *vector = map_add_var(retval, "assignments", 0);
  var_t *array = map_add_var(retval, "centroids", 0);

  v_toarray1(vector, assignments.n_cols);
  for (size_t c = 0; c < assignments.n_cols; c++) {
    v_setreal(v_elem(vector, c), assignments(0, c));
  }

  v_tomatrix(array, centroids.n_rows, centroids.n_cols);
  for (size_t r = 0; r < centroids.n_rows; r++) {
    for (size_t c = 0; c < centroids.n_cols; c++) {
      size_t pos = r * centroids.n_cols + c;
      v_setreal(v_elem(array, pos), centroids(r, c));
    }
  }
}

//
// The assignments vector indicates which cluster each data point
// belongs to (the indices of the vector correspond to the rows of the
// input data matrix), and the centroids matrix contains the
// coordinates of the cluster centroids.
//
// Scheme for clustering image data:
//
// Input - image bytes flattened to a '1D' array (2D for RGBA values)
// r|--------|
// g|--------|
// b|--------|
// a|--------|
//
// Output:
// 'centroids' vector indexes centroids to assignments
//  |00001100|
//
// 'assignments' matrix, holding a column position for centroid 0 or 1
// r|n|n|
// g|n|n|
// b|n|n|
// a|n|n|
//
static int cmd_kmeans_cluster(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int data_id = get_data_id(argc, params, 0, retval);
  if (data_id != -1) {
    size_t clusters = get_param_int(argc, params, 1, 5);
    size_t maxIterations = get_param_int(argc, params, 2, 1000);
    kmeans_cluster(*_dataMap.at(data_id), clusters, maxIterations, retval);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

static int cmd_neighbor_search(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int data_id = get_data_id(argc, params, 0, retval);
  if (data_id != -1) {
    neighbor_search(*_dataMap[data_id], retval);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

static int cmd_load(int argc, slib_par_t *params, var_t *retval) {
  int result;
  arma::mat *dataset = new arma::mat();
  if (is_param_array(argc, params, 0)) {
    result = get_mat(*dataset, params[0].var_p) ? 1 : 0;
  } else if (is_param_str(argc, params, 0)) {
    result = get_mat(*dataset, get_param_str(argc, params, 0, nullptr)) ? 1 : 0;
  } else {
    result = 0;
  }
  if (result) {
    v_setmat(retval, dataset);
  } else {
    delete dataset;
  }
  return result;
}

static int cmd_submat(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int data_id = get_data_id(argc, params, 0, retval);
  if (data_id != -1) {
    auto first_row = get_param_int(argc, params, 1, 0);
    auto first_col = get_param_int(argc, params, 2, 0);
    auto last_row = get_param_int(argc, params, 3, 0);
    auto last_col = get_param_int(argc, params, 4, 0);
    auto mat = _dataMap.at(data_id);
    arma::mat submat = mat->submat(first_row, first_col, last_row, last_col);
    v_setmat(retval, new arma::mat(submat));
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

static int cmd_split(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int data_id = get_data_id(argc, params, 0, retval);
  if (data_id != -1) {
    auto ratio = get_param_num(argc, params, 1, 0);
    arma::mat *train = new arma::mat();
    arma::mat *test = new arma::mat();
    data::Split(*_dataMap[data_id], *train, *test, ratio);
    map_init(retval);
    var_t *v_train = map_add_var(retval, "train", 0);
    var_t *v_test = map_add_var(retval, "test", 0);
    v_setmat(v_train, train);
    v_setmat(v_test, test);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

static int cmd_create_fnn_mse(int argc, slib_par_t *params, var_t *retval) {
  int id = ++_nextId;
  _ffnMseMap[id] = new FFN_MSE();
  map_init_id(retval, id, CLS_FFN_MSE);
  return 1;
}

static int cmd_create_fnn_nll(int argc, slib_par_t *params, var_t *retval) {
  int id = ++_nextId;
  _ffnNllMap[id] = new FFN_NLL();
  map_init_id(retval, id, CLS_FFN_NLL);
  return 1;
}

static int cmd_create_fnn_rl(int argc, slib_par_t *params, var_t *retval) {
  int id = ++_nextId;
  _ffnRlMap[id] = new FFN_RL();
  map_init_id(retval, id, CLS_FFN_RL);
  return 1;
}

static int cmd_get_row(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int data_id = get_data_id(argc, params, 0, retval);
  if (data_id != -1) {
    auto index = get_param_int(argc, params, 1, 0);
    arma::mat row = _dataMap[data_id]->row(index);
    v_setmat(retval, new arma::mat(row));
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

static int cmd_scale(int argc, slib_par_t *params, var_t *retval) {
  int result;
  int train_id = get_data_id(argc, params, 0, retval);
  int valid_id = get_data_id(argc, params, 1, retval);
  if (train_id != -1 && valid_id != -1) {
    arma::mat *train = _dataMap[train_id];
    arma::mat *valid = _dataMap[valid_id];
    data::MinMaxScaler scale;
    scale.Fit(*train);
    scale.Transform(*train, *train);
    scale.Transform(*valid, *valid);
    result = 1;
  } else {
    result = 0;
  }
  return result;
}

FUNC_SIG lib_func[] = {
  {0, 0, "FFN_MSE", cmd_create_fnn_mse},
  {0, 0, "FFN_NLL", cmd_create_fnn_nll},
  {0, 0, "FFN_RL", cmd_create_fnn_rl},
  {1, 1, "LOAD", cmd_load},
  {1, 1, "NEIGHBORSEARCH", cmd_neighbor_search},
  {2, 2, "GET_ROW", cmd_get_row},
  {2, 2, "SPLIT", cmd_split},
  {2, 3, "KMEANSCLUSTER", cmd_kmeans_cluster},
  {5, 5, "SUBMAT", cmd_submat}
};

FUNC_SIG lib_proc[] = {
  {2, 2, "SCALE", cmd_scale}
};

SBLIB_API int sblib_proc_count() {
  return (sizeof(lib_proc) / sizeof(lib_proc[0]));
}

SBLIB_API int sblib_func_count() {
  return (sizeof(lib_func) / sizeof(lib_func[0]));
}

SBLIB_API void sblib_free(int cls_id, int id) {
  if (id != -1) {
    switch (cls_id) {
    case CLS_DATA:
      if (_dataMap.find(id) != _dataMap.end()) {
        delete _dataMap.at(id);
        _dataMap.erase(id);
      }
      break;
    case CLS_FFN_MSE:
      if (_ffnMseMap.find(id) != _ffnMseMap.end()) {
        delete _ffnMseMap.at(id);
        _ffnMseMap.erase(id);
      }
      break;
    case CLS_FFN_NLL:
      if (_ffnNllMap.find(id) != _ffnNllMap.end()) {
        delete _ffnNllMap.at(id);
        _ffnNllMap.erase(id);
      }
      break;
    case CLS_FFN_RL:
      if (_ffnRlMap.find(id) != _ffnRlMap.end()) {
        delete _ffnRlMap.at(id);
        _ffnRlMap.erase(id);
      }
      break;
    }
  }
}

SBLIB_API void sblib_close(void) {
  if (!_dataMap.empty()) {
    fprintf(stderr, "Matrix leak detected\n");
  }
  if (!_ffnMseMap.empty()) {
    fprintf(stderr, "FFN MSE leak detected\n");
  }
  if (!_ffnNllMap.empty()) {
    fprintf(stderr, "FFN NLL leak detected\n");
  }
  if (!_ffnRlMap.empty()) {
    fprintf(stderr, "FFN RL leak detected\n");
  }
}
