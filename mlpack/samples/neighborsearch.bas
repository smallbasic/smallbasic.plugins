import mlpack

mat = [[0.339406815, 0.843176636, 0.472701471, 0.212587646],&
       [0.351174901, 0.81056695,  0.160147626, 0.255047893],&
       [0.04072469,  0.564535197, 0.943435462, 0.597070812]]

result = mlpack.neighborSearch(mat)

for i = 0 to len(result.neighbors) - 1
  print "Nearest neighbor of point "; i; " is point "; result.neighbors[i]; " and the distance is "; result.distances[i]; "."
next  

