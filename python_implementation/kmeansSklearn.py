from sklearn.datasets import make_blobs
import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
from sklearn.cluster import KMeans
import seaborn as sns
import sys


X = pd.read_csv("uniform_small_2d_lowstddev/uniform_small_2d_lowstddev_samples.csv", names = ['F1', 'F2']).to_numpy()

cluster_centers = np.array([[-1.08756, 0.480849], [-3.12783, -1.43567], [-4.99813, -1.80588], [-3.5161, -4.12267], [-3.60242, -4.02816], [-0.912942, 0.309218], [-3.4735, -4.13939]])


kmeans_kwargs = {
    "n_clusters": 7,
    "init": cluster_centers, # How centroids will be initialized
    "n_init": 1, # How many times k-means will be run with != starting
    "verbose": 1,
    "tol": 0.0001,
    "algorithm": "lloyd",
}

with open('out.txt', 'w') as f:

    def tracefunc(frame, event, arg, indent=[0]):
        if event == "call":
            indent[0] += 2
            print("-" * indent[0] + "> call function", frame.f_code.co_name)
        elif event == "return":
            print("<" + "-" * indent[0], "exit function", frame.f_code.co_name)
            indent[0] -= 2
        return tracefunc

sys.setprofile(tracefunc)

kmeans = KMeans( **kmeans_kwargs)
A = kmeans.fit(X)
