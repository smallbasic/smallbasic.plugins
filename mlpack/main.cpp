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
#include <mlpack/methods/kmeans/kmeans.hpp>

// https://arma.sourceforge.net/docs.html
using namespace mlpack;

void error_array(var_p_t var) {
  error(var, "First argument must be an array [or image file name]");
}

//
// load from CSV
//
static bool get_mat(arma::mat &dataset, const char *str) {
  return data::Load(str, dataset, true);
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

static void cmd_kmeans_cluster(arma::mat &dataset, size_t clusters, size_t maxIterations, var_t *retval) {
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
  int result = 0;
  if (!is_param_num(argc, params, 1)) {
    error(retval, "Second argument must be an integer");
  } else {
    arma::mat dataset;
    if (is_param_array(argc, params, 0)) {
      result = get_mat(dataset, params[0].var_p) ? 1 : 0;
    } else if (is_param_str(argc, params, 0)) {
      result = get_mat(dataset, get_param_str(argc, params, 0, nullptr)) ? 1 : 0;
    }
    if (result) {
      size_t clusters = get_param_int(argc, params, 1, 5);
      size_t maxIterations = get_param_int(argc, params, 2, 1000);
      cmd_kmeans_cluster(dataset, clusters, maxIterations, retval);
    } else {
      error_array(retval);
    }
  }
  return result;
}

static void cmd_neighbor_search(arma::mat &dataset, var_t *retval) {
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

static int cmd_neighbor_search(int argc, slib_par_t *params, var_t *retval) {
  int result;
  arma::mat dataset;
  if (is_param_array(argc, params, 0)) {
    result = get_mat(dataset, params[0].var_p) ? 1 : 0;
  } else if (is_param_str(argc, params, 0)) {
    result = get_mat(dataset, get_param_str(argc, params, 0, nullptr)) ? 1 : 0;
  } else {
    result = 0;
  }
  if (result) {
    cmd_neighbor_search(dataset, retval);
  } else {
    error_array(retval);
  }
  return result;
}

FUNC_SIG lib_func[] = {
  {2, 3, "KMEANSCLUSTER", cmd_kmeans_cluster},
  {1, 1, "NEIGHBORSEARCH", cmd_neighbor_search},
};

FUNC_SIG lib_proc[] = {
};

SBLIB_API int sblib_proc_count() {
  return 0;
}

SBLIB_API int sblib_func_count() {
  return (sizeof(lib_func) / sizeof(lib_func[0]));
}
