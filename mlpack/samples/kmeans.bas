'
' k-means clustering can be used in image detection and segmentation. 
' One application of k-means clustering in image processing is to group
' similar pixels in an image into clusters,  which can then be used 
' for tasks such as image segmentation, object recognition, and compression.

' For example, suppose you have an image of a landscape with different
' types of vegetation and you want to separate the pixels corresponding
' to each type of vegetation. You can use k-means clustering to group
' similar pixels into clusters, where each cluster corresponds to a type
' of vegetation. The cluster centers or centroids can then be used as a
' representative color for each type of vegetation.
'

' https://www.naftaliharris.com/blog/visualizing-k-means-clustering/

import mlpack

DIM arr(0 to 2, 0 to 7)

arr(0, 0) = .11
arr(0, 1) = .22
arr(0, 2) = .33
arr(0, 3) = 99

arr(1, 0) = .44
arr(1, 1) = .55
arr(1, 2) = .66
arr(1, 3) = 100

arr(2, 0) = .77
arr(2, 1) = .88
arr(2, 2) = .99
arr(2, 3) = 101

sub show(byref a)
  local h = ubound(a, 1)
  local w = ubound(a, 2)
  local x, y

  print a
  print "rows=";h+1; " cols=";w+1
  for y = 0 to h
    for x = 0 to w
      print "  "; a[y, x]; ", ";
    next
    print
  next
end  

show arr

kresult = mlpack.kmeansCluster(arr, 2)
? "assignments:"
? kresult.assignments

? "centroids:"
show kresult.centroids
