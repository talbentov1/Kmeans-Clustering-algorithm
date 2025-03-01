#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include "symnmf.h"

// In all this functions there is no actuall implementation only hamarot input and calling to the sutiable functions from symnmf.c

static double **convertArrayOfArraysFromPythonToC(PyObject *outer_list, int row_size, int col_size)
{
    int i, j;
    double **CArray = calloc(row_size, sizeof(double *));
    if (CArray == NULL)
    {
        return NULL;
    }
    for (i = 0; i < row_size; i++)
    {
        CArray[i] = calloc(col_size, sizeof(double));
        if (CArray[i] == NULL)
        {
            freeArrayOfArraysMemory(CArray, i - 1);
            return NULL;
        }
        PyObject *inner_list = PyList_GetItem(outer_list, i);
        for (j = 0; j < col_size; j++)
        {
            CArray[i][j] = PyFloat_AsDouble(PyList_GetItem(inner_list, j));
        }
    }
    return CArray;
}
/* Allocates memory of ArrayOfArrays in C */
static PyObject *convertArrayOfArraysFromCToPython(double **ArrayOfArrays, int row_size, int col_size)
{
    PyObject *python_final_result = PyList_New(row_size);
    int i, j;
    for (i = 0; i < row_size; i++)
    {
        PyObject *python_result_row = PyList_New(col_size);
        for (j = 0; j < col_size; j++)
        {
            PyList_SET_ITEM(python_result_row, j, PyFloat_FromDouble(ArrayOfArrays[i][j]));
        }
        PyList_SET_ITEM(python_final_result, i, python_result_row);
        free(ArrayOfArrays[i]);
    }
    free(ArrayOfArrays);
    return python_final_result;
}

static PyObject *symnmfModule(PyObject *self, PyObject *args)
{
    double **W, **H, **afterH;
    PyObject *outer_list_W, *outer_list_H, *python_final_result_H;
    int n, k;
    /*read arguments from python (A) and converting them to C: */
    if (!PyArg_ParseTuple(args, "OOii", &outer_list_W, &outer_list_H, &n, &k))
    {
        PyErr_SetString(PyExc_ValueError, "An Error Has Occurred");
        return NULL;
    }
    W = convertArrayOfArraysFromPythonToC(outer_list_W, n, n);
    H = convertArrayOfArraysFromPythonToC(outer_list_H, n, k);
    if (W == NULL || H == NULL){
        PyErr_SetString(PyExc_ValueError, "An Error Has Occurred");
        return NULL;
    }
    /*calculating H in C language:*/
    afterH = symnmf(W, H, n, k, 0.0001, 300);
    freeArrayOfArraysMemory(W, n);
    freeArrayOfArraysMemory(H, n);
    /*if calculating H failed, exit: */
    if (afterH == NULL)
    {
        PyErr_SetString(PyExc_ValueError, "An Error Has Occurred");
        return NULL;
    }
    /*converting the result (H) from C to python and freeing the result's memory in C: */
    python_final_result_H = convertArrayOfArraysFromCToPython(afterH, n, k);
    return python_final_result_H;
}

static PyObject *symModule(PyObject *self, PyObject *args)
{
    double **datapoints, **A;
    PyObject *outer_list_datapoints, *python_final_result_A;
    int datapointsListLen, datapointLen;
    /*read arguments (datapoints) from python and converting them to C: */
    if (!PyArg_ParseTuple(args, "Oii", &outer_list_datapoints, &datapointsListLen, &datapointLen))
    {
        PyErr_SetString(PyExc_ValueError, "An Error Has Occurred");
        return NULL;
    }
    datapoints = convertArrayOfArraysFromPythonToC(outer_list_datapoints, datapointsListLen, datapointLen);
    if (datapoints == NULL)
    {
        PyErr_SetString(PyExc_ValueError, "An Error Has Occurred");
        return NULL;
    }
    /*calculating A in C language:*/
    A = sym(datapoints, datapointsListLen, datapointLen);
    freeArrayOfArraysMemory(datapoints, datapointsListLen);
    /*if calculating A failed, exit: */
    if (A == NULL)
    {
        PyErr_SetString(PyExc_ValueError, "An Error Has Occurred");
        return NULL;
    }
    /*converting the result (A) from C to python and freeing the result's memory in C: */
    python_final_result_A = convertArrayOfArraysFromCToPython(A, datapointsListLen, datapointsListLen);
    return python_final_result_A;
}

static PyObject *ddgModule(PyObject *self, PyObject *args)
{
    double **D, **A;
    PyObject *outer_list_A, *python_final_result_D;
    int datapointsListLen;
    /*read arguments from python (A) and converting them to C: */
    if (!PyArg_ParseTuple(args, "Oi", &outer_list_A, &datapointsListLen))
    {
        PyErr_SetString(PyExc_ValueError, "An Error Has Occurred");
        return NULL;
    }
    A = convertArrayOfArraysFromPythonToC(outer_list_A, datapointsListLen, datapointsListLen);
    if (A == NULL)
    {
        PyErr_SetString(PyExc_ValueError, "An Error Has Occurred");
        return NULL;
    }
    /*calculating D in C language:*/
    D = ddg(A, datapointsListLen);
    freeArrayOfArraysMemory(A, datapointsListLen);
    /*if calculating D failed, exit: */
    if (D == NULL)
    {
        PyErr_SetString(PyExc_ValueError, "An Error Has Occurred");
        return NULL;
    }
    /*converting the result (D) from C to python and freeing the result's memory in C: */
    python_final_result_D = convertArrayOfArraysFromCToPython(D, datapointsListLen, datapointsListLen);
    return python_final_result_D;
}

static PyObject *normModule(PyObject *self, PyObject *args)
{
    double **D, **A, **W;
    PyObject *outer_list_A, *outer_list_D, *python_final_result_W;
    int datapointsListLen;
    /*read arguments from python (A, D) and converting them to C: */
    if (!PyArg_ParseTuple(args, "OOi", &outer_list_A, &outer_list_D, &datapointsListLen))
    {
        PyErr_SetString(PyExc_ValueError, "An Error Has Occurred");
        return NULL;
    }
    A = convertArrayOfArraysFromPythonToC(outer_list_A, datapointsListLen, datapointsListLen);
    D = convertArrayOfArraysFromPythonToC(outer_list_D, datapointsListLen, datapointsListLen);
    if (A == NULL || D == NULL){
        PyErr_SetString(PyExc_ValueError, "An Error Has Occurred");
        return NULL;
    }
    /*calculating W in C language:*/
    W = norm(A, D, datapointsListLen);
    /*freeing memory of A, D happens in function norm*/
    /*if calculating W failed, exit: */
    if (W == NULL)
    {
        PyErr_SetString(PyExc_ValueError, "An Error Has Occurred");
        return NULL;
    }
    /*converting the result (W) from C to python and freeing the result's memory in C: */
    python_final_result_W = convertArrayOfArraysFromCToPython(W, datapointsListLen, datapointsListLen);
    return python_final_result_W;
}

//////////////////////////////////////////////////////////////////////////////////
// module's function table
static PyMethodDef symnmfMethods[] = {
    {
        "symnmfModule",                                           // name exposed to Python
        (PyCFunction)symnmfModule,                                // C wrapper function
        METH_VARARGS,                                             // received variable args (but really just 1)
        PyDoc_STR("calculates SynNMF for a given datapoints set") // documentation
    },
    {
        "symModule",
        (PyCFunction)symModule,
        METH_VARARGS,
        PyDoc_STR("calculates the similarity matrix of a given datapoints set") // documentation
    },
    {
        "ddgModule",
        (PyCFunction)ddgModule,
        METH_VARARGS,
        PyDoc_STR("calculates the diagonal degree matrix of a given datapoints set") // documentation
    },
    {
        "normModule",
        (PyCFunction)normModule,
        METH_VARARGS,
        PyDoc_STR("calculates the normalized similarity matrix of a given datapoints set") // documentation
    },
    {NULL, NULL, 0, NULL}};

//////////////////////////////////////////////////////////////////////////////
// modules definition
static struct PyModuleDef symnmfmoudle = {
    PyModuleDef_HEAD_INIT,
    "mySymNmf", // name of module exposed to Python
    NULL,       // module documentation
    -1,
    symnmfMethods};

PyMODINIT_FUNC PyInit_mySymNmf(void)
{
    PyObject *m;
    m = PyModule_Create(&symnmfmoudle);
    if (!m)
    {
        return NULL;
    }
    return m;
}
