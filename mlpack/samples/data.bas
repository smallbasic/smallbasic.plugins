const RATIO = 0.1 ' 10%

import mlpack
i = image("chicken.png")
i.save(a)

dataset = mlpack.Load(a)

' Split the dataset into training and validation sets.
split_data = mlpack.split(dataset, RATIO)
trainData = split_data.train
validData = split_data.test

' The train and valid datasets contain both - the features as well as the
' prediction. Split these into separate matrices.
trainX = mlpack.submat(trainData, 1, 1, trainData.n_rows - 1, trainData.n_cols - 1)
validX = mlpack.submat(validData, 1, 1, validData.n_rows - 1, validData.n_cols - 1)

' Create prediction data for training and validatiion datasets.
trainY = mlpack.get_row(trainData, 0)
validY = mlpack.get_row(validData, 0)

' Scale all data into the range (0, 1) for increased numerical stability.
mlpack.scale(trainX, validX)
mlpack.scale(trainY, validY)

print "done!"

