import sys
import math
import pandas as pd
import numpy as np
import mySymNmf

def readFile (fileName):
    datapoints_table = pd.read_csv(fileName, sep=",", header=None)
    #datapointsListLen = num of datapoints :
    datapointsListLen = datapoints_table.shape[0]
    datapointLen = datapoints_table.shape[1]
    # turn into a list:
    datapoints = datapoints_table.values.tolist()
    return (datapoints, datapointsListLen, datapointLen)

def initializeH (W, n, k):
    np.random.seed(0)
    m = matrixAvg(W)
    H = [[np.random.uniform(0, 2 * math.sqrt(m/k)) for j in range(k)] for i in range(n)]
    return H

def matrixAvg (matrix):
    sum = 0
    count = 0
    for row in range(len(matrix)):
        for col in range(len(matrix[row])):
            sum += matrix[row][col]
            count +=1
    return sum / count


def printFinalResult (matrix):
    for row in matrix:
        for col in range(len(row)):
            if(col == len(row) - 1):
                print("{:.4f}".format(row[col]), end="")
            else:    
                print("{:.4f}".format(row[col]), end=",")
        print("")


if(len(sys.argv) == 4):
    k = int(sys.argv[1])
    goal = sys.argv[2]
    fileName = sys.argv[3]
    (datapoints, datapointsListLen, datapointLen) = readFile(fileName)
    if goal == "symnmf":
        A = mySymNmf.symModule(datapoints, datapointsListLen, datapointLen)
        D = mySymNmf.ddgModule(A, datapointsListLen)
        W = mySymNmf.normModule(A, D, datapointsListLen)
        Hinit = initializeH(W, datapointsListLen, k)
        H = mySymNmf.symnmfModule(W, Hinit, datapointsListLen, k)
        printFinalResult(H)
    if goal == "sym":
        A = mySymNmf.symModule(datapoints, datapointsListLen, datapointLen)
        printFinalResult(A)
    if goal == "ddg":
        A = mySymNmf.symModule(datapoints, datapointsListLen, datapointLen)
        D = mySymNmf.ddgModule(A, datapointsListLen)
        printFinalResult(D)
    if goal == "norm":
        A = mySymNmf.symModule(datapoints, datapointsListLen, datapointLen)
        D = mySymNmf.ddgModule(A, datapointsListLen)
        W = mySymNmf.normModule(A, D, datapointsListLen)
        printFinalResult(W)

    