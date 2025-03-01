import sys
from kmeans import kMeans
from sklearn.metrics import silhouette_score
from symnmf import readFile, initializeH
import mySymNmf

def getClustersFromH (H):
    clusters = []
    for row in H:
        clusters.append(row.index(max(row)))
    return clusters


if(len(sys.argv) == 3):
    k = int(sys.argv[1])
    fileName = sys.argv[2]
    (datapoints, datapointsListLen, datapointLen) = readFile(fileName)
    kMeansClusters = kMeans(datapoints, datapointsListLen, k, 300)
    # compute symNMF:
    A = mySymNmf.symModule(datapoints, datapointsListLen, datapointLen)
    D = mySymNmf.ddgModule(A, datapointsListLen)
    W = mySymNmf.normModule(A, D, datapointsListLen)
    Hinit = initializeH(W, datapointsListLen, k)
    H = mySymNmf.symnmfModule(W, Hinit, datapointsListLen, k)
    symNmfClusters = getClustersFromH (H)
    print ("nmf: ", "{:.4f}".format(silhouette_score (datapoints, symNmfClusters)))
    print ("kmeans: ", "{:.4f}".format(silhouette_score (datapoints, kMeansClusters)))
    


