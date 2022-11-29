from sklearn.datasets import make_blobs
import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
from sklearn.cluster import KMeans
import seaborn as sns
import sys


X = pd.read_csv("uniform_large_2d_lowstddev_samples.csv", names = ['F1', 'F2']).to_numpy()

cluster_centers = np.array([ [-0.8297, 2.2032],
    [-4.9989, -1.9767],
    [-3.5324, -4.0766],
    [-3.1374, -1.5444],
    [-1.0323, 0.3882],
    [-0.8081, 1.8522],
    [-2.9555, 3.7811]])


kmeans_kwargs = {
    "n_clusters": 7,
    "init": "random", # How centroids will be initialized
    "n_init": 1, # How many times k-means will be run with != starting
    "tol": 0,
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

#sys.setprofile(tracefunc)

kmeans = KMeans( **kmeans_kwargs)
A = kmeans.fit(X)
