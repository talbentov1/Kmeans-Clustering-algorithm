import sys;
import math;

def kMeans(k, textFile, iter = 200):
    with open(textFile) as f:
        datapointsString = f.read()
    f.close()
    datapoints = datapointsString.split("\n")
    #removing last /n:
    datapoints = datapoints[:-1]
    N = len(datapoints)
    for i in range(N):
        #splitting each datapoint to a list:
        datapoints[i] = datapoints[i].split(",")
        #casting each point to a float:
        for j in range(len(datapoints[i])):
            datapoints[i][j] = float(datapoints[i][j])
    #checking validation:
    if (k <= 1 or k >= N) or (iter <= 1 or iter >= 1000):
        if k <= 1 or k >= N:
            print ("Invalid number of clusters!")
        if iter <= 1 or iter >= 1000:
            print ("Invalid maximum iteration!")
        return False
    centroids = []
    #initializing centroids to the first k datapoints:
    for i in range(k):
        centroids.append(datapoints[i])
    iter_number = 0
    while iter_number < iter:
        #datapoints_clusters[i] will be equal to the cluster of datapoint i:
        datapoints_clusters = assignClusters(datapoints, centroids)
        new_centroids = updateCentroids(datapoints, datapoints_clusters, k)
        #deltaCentroidCheck() returns true iff deltaCentroid-k < EPSILON for every k in centroids
        if deltaCentroidCheck(centroids, new_centroids):
            break
        centroids = new_centroids
        iter_number += 1
    printfinalresult(centroids)
    return True

def assignClusters (datapoints, centroids):
    #at the end of running, clusters[i] will be equal to the cluster of datapoint i
    clusters = []
    for point in datapoints:
        distances = [euclideanDistance(point, centroid) for centroid in centroids]
        cluster_index = distances.index(min(distances))
        clusters.append(cluster_index)
    return clusters


def updateCentroids(datapoints, datapoints_clusters, k):
    new_centroids = []
    #for every centroid:
    for i in range(k):
        #putting all datapoints that belong to the i'th cluster in cluster_points:
        cluster_points = [datapoints[j] for j in range(len(datapoints)) if datapoints_clusters[j] == i]
        #calculate new centroid from all datapoints that belong to the i'th cluster:
        centroid = calculateCentroid(cluster_points)
        #adding the new centroid to new_centroids list:
        new_centroids.append(centroid)
    return new_centroids

def calculateCentroid(cluster_points):
    points_number = len(cluster_points)
    point_len = len(cluster_points[0])
    centroid = []
    for i in range(point_len):
        sum_coordinate = sum(cluster_points[j][i] for j in range(points_number))
        centroid.append(sum_coordinate / points_number)
    return centroid           

def euclideanDistance(point1, point2):
    return math.sqrt(sum((p1 - p2) ** 2 for p1, p2 in zip(point1, point2)))
    
def deltaCentroidCheck(new, old):
    for i in range(len(new)):
        if euclideanDistance(new[i], old[i]) > EPSILON:
            return False
            
def printfinalresult(centroids):
    for centroid in centroids:
        for i in range(len(centroid)):
            if(i == len(centroid) - 1):
                print("{:.4f}".format(centroid[i]), end="")
            else:    
                print("{:.4f}".format(centroid[i]), end=",")
        print("")
    


EPSILON = 0.001
if(len(sys.argv) == 4):
    k_input = sys.argv[1]
    iter_input = sys.argv[2]
    data_input = sys.argv[3]
    kMeans(int(k_input), data_input, int(iter_input))
else:
    k_input = sys.argv[1]
    data_input = sys.argv[2]
    kMeans(int(k_input), data_input)




