#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

struct cord
{
    double value;
    struct cord *next;
};
struct datapoint
{
    struct datapoint *next;
    struct cord *cords;
    int cluster;
};

typedef struct datapoint datapoint;
typedef struct cord cord;

double **centroids;
double **new_centroids;

int initializeNewCentroids(datapoint *, int, int);
void assignClusters(datapoint *, int);
double euclideanDistance(cord *, double *);
double euclideanDistanceArr(double *, double *, int);
int updateCentroids(datapoint *, int, int);
int deltaCentroidCheck(double, int, int);
void freeMemory(datapoint *, int, int);
void swapCentroids(double ***, double ***);
int kmeans(datapoint *, int, double, int, int);

int kmeans(datapoint *head, int iter, double epsilon, int clusterNum, int datapointLen)
{
    int iter_num = 0;
    /*initialize new centroids array*/
    if (!initializeNewCentroids(head, clusterNum, datapointLen))
    {
        return 0;
    }

    while (iter_num < iter)
    {
        assignClusters(head, clusterNum);
        if (!updateCentroids(head, clusterNum, datapointLen))
        {
            return 0;
        }
        if (deltaCentroidCheck(epsilon, datapointLen, clusterNum) == 1)
        {
            break;
        }
        swapCentroids(&centroids, &new_centroids);
        iter_num++;
    }
    freeMemory(head, 0, clusterNum);
    return 1;
}

static PyObject *fit(PyObject *self, PyObject *args)
{
    datapoint *head_vec = NULL, *curr_vec, *prev_vec;
    cord *head_cord = NULL, *curr_cord, *prev_cord;
    PyObject *outer_list_centroids;
    PyObject *outer_list_datapoints;
    int inner_len = 0, outer_len = 0;
    int iter;
    double epsilon;
    PyObject *python_final_centroids;
    PyObject *python_vector_list;

    if (!PyArg_ParseTuple(args, "OOid", &outer_list_centroids, &outer_list_datapoints, &iter, &epsilon))
    {
        return NULL;
    }

    // get datapoints from python
    outer_len = PyObject_Length(outer_list_datapoints);
    if (outer_len < 0)
    {
        return NULL;
    }
    for (int i = 0; i < outer_len; i++)
    {
        PyObject *inner_list = PyList_GetItem(outer_list_datapoints, i);
        inner_len = PyObject_Length(inner_list);
        if (inner_len < 0)
        {
            return NULL;
        }
        curr_vec = malloc(sizeof(datapoint));
        if (curr_vec == NULL)
        {
            freeMemory(head_vec, 0, 0);
            PyErr_SetString(PyExc_ValueError, "An Error Has Occurred");
            return NULL;
        }
        curr_vec->next = NULL;
        if (i == 0)
        {
            head_vec = curr_vec;
        }
        else
        {
            prev_vec->next = curr_vec;
        }
        for (int j = 0; j < inner_len; j++)
        {
            curr_cord = malloc(sizeof(cord));
            if (curr_cord == NULL)
            {
                freeMemory(head_vec, 0, 0);
                PyErr_SetString(PyExc_ValueError, "An Error Has Occurred");
                return NULL;
            }
            curr_cord->next = NULL;
            if (j == 0)
            {
                head_cord = curr_cord;
            }
            else
            {
                prev_cord->next = curr_cord;
            }
            PyObject *datapoint_coordinate = PyList_GetItem(inner_list, j);
            curr_cord->value = PyFloat_AsDouble(datapoint_coordinate);
            prev_cord = curr_cord;
        }
        curr_vec->cords = head_cord;
        prev_vec = curr_vec;
    }

    // get centroids from python
    outer_len = PyObject_Length(outer_list_centroids);
    if (outer_len < 0)
    {
        return NULL;
    }

    centroids = calloc(outer_len, sizeof(double *));
    if (centroids == NULL)
    {
        freeMemory(head_vec, 0, 0);
        PyErr_SetString(PyExc_ValueError, "An Error Has Occurred");
        return NULL;
    }
    for (int i = 0; i < outer_len; i++)
    {
        PyObject *inner_list = PyList_GetItem(outer_list_centroids, i);
        inner_len = PyObject_Length(inner_list);
        if (inner_len < 0)
        {
            return NULL;
        }
        centroids[i] = calloc(inner_len, sizeof(double));
        if (centroids[i] == NULL)
        {
            freeMemory(head_vec, i - 1, 0);
            PyErr_SetString(PyExc_ValueError, "An Error Has Occurred");
            return NULL;
        }
        for (int j = 0; j < inner_len; j++)
        {
            PyObject *coordinate_py = PyList_GetItem(inner_list, j);
            centroids[i][j] = PyFloat_AsDouble(coordinate_py);
        }
    }
    if (!kmeans(head_vec, iter, epsilon, outer_len, inner_len))
    {
        PyErr_SetString(PyExc_ValueError, "An Error Has Occurred");
        return NULL;
    }
    python_final_centroids = PyList_New(outer_len);
    for (int i = 0; i < outer_len; i++)
    {
        python_vector_list = PyList_New(inner_len);
        for (int j = 0; j < inner_len; j++)
        {
            PyList_SET_ITEM(python_vector_list, j, PyFloat_FromDouble(centroids[i][j]));
        }
        PyList_SET_ITEM(python_final_centroids, i, python_vector_list);
        free(centroids[i]);
    }
    free(centroids);
    return python_final_centroids;
}

static PyMethodDef kmeansMethods[] = {
    {"fit",
     (PyCFunction)fit,
     METH_VARARGS,
     PyDoc_STR("implementation of kmeans algorithm using given centroids, datapoints, iter and epsilon")},
    {NULL, NULL, 0, NULL}

};

static struct PyModuleDef kmeansmodule = {
    PyModuleDef_HEAD_INIT,
    "mykmeanssp",
    NULL,
    -1,
    kmeansMethods

};

PyMODINIT_FUNC PyInit_mykmeanssp(void)
{
    PyObject *m;
    m = PyModule_Create(&kmeansmodule);
    if (!m)
    {
        return NULL;
    }
    return m;
}

int initializeNewCentroids(datapoint *head, int clusterNum, int datapointLen)
{
    new_centroids = calloc(clusterNum, sizeof(double *));
    if (new_centroids == NULL)
    {
        freeMemory(head, clusterNum, 0);
        return 0;
    }
    for (int i = 0; i < clusterNum; i++)
    {
        new_centroids[i] = calloc(datapointLen, sizeof(double));
        if (new_centroids[i] == NULL)
        {
            freeMemory(head, clusterNum, i - 1);
            return 0;
        }
    }
    return 1;
}

void assignClusters(datapoint *head, int clusterNum)
{
    int min_cluster;
    double min_distance, curr_distance;
    int i;
    while (head != NULL)
    {
        min_distance = euclideanDistance(head->cords, centroids[0]);
        min_cluster = 0;
        for (i = 1; i < clusterNum; i++)
        {
            curr_distance = euclideanDistance(head->cords, centroids[i]);
            if (curr_distance < min_distance)
            {
                min_cluster = i;
                min_distance = curr_distance;
            }
        }
        head->cluster = min_cluster;
        head = head->next;
    }
}

double euclideanDistance(cord *cord, double *cords)
{
    double sum = 0;
    int i = 0;
    while (cord != NULL)
    {
        sum += pow((cord->value - cords[i]), 2);
        cord = cord->next;
        i++;
    }
    return sqrt(sum);
}

double euclideanDistanceArr(double *arr1, double *arr2, int datapointLen)
{
    double sum = 0;
    int i;
    for (i = 0; i < datapointLen; i++)
    {
        sum += pow((arr1[i] - arr2[i]), 2);
    }
    return sqrt(sum);
}

int updateCentroids(datapoint *head, int clusterNum, int datapointLen)
{
    int i, j, t;
    int points_counter = 0;
    datapoint *curr = head;
    cord *tmp;
    double *sum_coordinates = calloc(datapointLen, sizeof(double));
    if (sum_coordinates == NULL)
    {
        freeMemory(head, clusterNum, clusterNum);
        return 0;
    }
    /*for every centroid: */
    for (i = 0; i < clusterNum; i++)
    {
        /*for every datapoint in this centroid's cluster: */
        while (curr != NULL)
        {
            if (curr->cluster == i)
            {
                points_counter++;
                tmp = curr->cords;
                for (j = 0; j < datapointLen; j++)
                {
                    sum_coordinates[j] += tmp->value;
                    tmp = tmp->next;
                }
            }
            curr = curr->next;
        }
        for (t = 0; t < datapointLen; t++)
        {
            new_centroids[i][t] = sum_coordinates[t] / points_counter;
            sum_coordinates[t] = 0;
        }
        curr = head;
        points_counter = 0;
    }
    free(sum_coordinates);
    return 1;
}

int deltaCentroidCheck(double epsilon, int datapointLen, int clusterNum)
{
    int i;
    /*for every centroid: */
    for (i = 0; i < clusterNum; i++)
    {
        if (euclideanDistanceArr(centroids[i], new_centroids[i], datapointLen) > epsilon)
        {
            return 0;
        }
    }
    return 1;
}

void freeMemory(datapoint *head, int centroids_size, int new_centroids_size)
{
    int i;
    datapoint *tmp_datapoint;
    cord *tmp_cord;
    while (head != NULL)
    {
        while (head->cords != NULL)
        {
            tmp_cord = head->cords;
            head->cords = head->cords->next;
            free(tmp_cord);
        }
        tmp_datapoint = head;
        head = head->next;
        free(tmp_datapoint);
    }

    /*free new_centroids: */
    if (new_centroids_size > 0)
    {
        for (i = 0; i < new_centroids_size; i++)
        {
            free(new_centroids[i]);
        }
        free(new_centroids);
    }

    /*free centroids: */
    if (centroids_size > 0)
    {
        for (i = 0; i < centroids_size; i++)
        {
            free(centroids[i]);
        }
        free(centroids);
    }
}

void swapCentroids(double ***centroids1, double ***centroids2)
{
    double **temp = *centroids1;
    *centroids1 = *centroids2;
    *centroids2 = temp;
}
