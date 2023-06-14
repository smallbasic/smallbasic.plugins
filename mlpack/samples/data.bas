import mlpack
i = image("chicken.png")
i.save(a)
dataset = mlpack.Load(a)
print dataset
print mlpack.submat(dataset, 1, 1, dataset.n_rows - 1, dataset.n_cols - 1)
