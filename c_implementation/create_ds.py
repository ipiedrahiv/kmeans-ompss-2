from sklearn.datasets import make_blobs
import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
import seaborn as sns

def create_arrays(n_samples, n_features, centers, cluster_std, center_box, random_state):
    X, y, c = make_blobs(n_samples=n_samples, n_features=n_features, centers=centers, cluster_std=cluster_std, center_box=center_box, random_state=random_state, return_centers=True)
    return X, y, c

# Create a dataset with uniform, 2d, no-overlap, k = 7 clusters
X, y, c = create_arrays(1000, 10, 4, 0.1, [-100000, 100000], 1)


np.savetxt("uniform_small_10d_samples.csv", X, delimiter=",")
np.savetxt("uniform_small_10d_labels.csv", y, delimiter=",")
np.savetxt("uniform_small_10d_centers.csv", c, delimiter=",")
