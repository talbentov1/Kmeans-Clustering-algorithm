import sys
import math
import pandas as pd
import numpy as np
import mykmeanssp

def kMeansPP(k, epsilon, textFile1, textFile2, iter = 300):
    table1 = pd.read_csv(textFile1, sep=",", header=None)
    table2 = pd.read_csv(textFile2, sep=",", header=None)
    datapoints_table = pd.merge(table1, table2, on=0)
    #N = num of datapoints
    N = datapoints_table.shape[0]
    #checking validation:
    if (k <= 1 or k >= N) or (iter <= 1 or iter >= 1000):
        if k <= 1 or k >= N:
            print ("Invalid number of clusters!")
        if iter <= 1 or iter >= 1000:
            print ("Invalid maximum iteration!")
        return False
    datapoints_table = datapoints_table.sort_values(by = 0)
    # pop first column (=key):
    datapoints_table.pop(0)
    # turn into a list:
    datapoints = datapoints_table.values.tolist()
    (centroids, centers_idx) = initializeKCentroids(datapoints.copy(), k)
    printFinalResult(centers_idx, mykmeanssp.fit(centroids, datapoints, iter, epsilon))

def printFinalResult(centers_idx, centroids):
    for i in range(len(centers_idx)):
        if i == len(centers_idx) - 1:
            print(centers_idx[i], end="")
        else:    
            print(centers_idx[i], end=",")
    print("")
    for centroid in centroids:
        for i in range(len(centroid)):
            if(i == len(centroid) - 1):
                print("{:.4f}".format(centroid[i]), end="")
            else:    
                print("{:.4f}".format(centroid[i]), end=",")
        print("")

def initializeKCentroids(datapoints, k):
    centers = []
    centers_idx = []
    # Step 1: Choose one center uniformly at random
    initial_center_idx = np.random.choice(len(datapoints))
    centers.append(datapoints[initial_center_idx])
    centers_idx.append(initial_center_idx)

    while len(centers) < k:
        probabilities = np.zeros(len(datapoints))
        # Step 2: Compute the distance between each data point and the nearest center
        for i in range(len(datapoints)):
            # calculate euclidean distance:
            if i not in centers_idx:
                nearest_center_distance = minDist(np.array(datapoints[i]), centers)
                probabilities[i] = nearest_center_distance

        
        # Step 3: Choose a new data point as a new center using weighted probability distribution
        probabilities = probabilities / np.sum(probabilities)
        new_center_idx = np.random.choice(len(datapoints), p=probabilities)
        new_center = datapoints[new_center_idx]
        centers.append(new_center)
        centers_idx.append(new_center_idx)
    
    return (centers, centers_idx)
    
def minDist(point, centers):
    min_dist = math.inf
    for center in centers:
        dist = np.linalg.norm(point - np.array(center))
        if dist < min_dist:
            min_dist = dist
    return min_dist

np.random.seed(0)
if(len(sys.argv) == 6):
    k = sys.argv[1]
    iter = sys.argv[2]
    epsilon = sys.argv[3]
    data1 = sys.argv[4]
    data2 = sys.argv[5]
    kMeansPP(int(k), float(epsilon), data1, data2, int(iter))
elif(len(sys.argv) == 5):
    k = sys.argv[1]
    epsilon = sys.argv[2]
    data1 = sys.argv[3]
    data2 = sys.argv[4]
    kMeansPP(int(k), float(epsilon), data1, data2)
else:
    print("An Error Has Occurred")