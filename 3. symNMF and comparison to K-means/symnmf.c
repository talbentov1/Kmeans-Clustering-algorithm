#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "symnmf.h"

/*In this file we implement the actuall code - the C independent interface + calaculations.*/

datapoint *readText(char *);
void freeLinkedListMemory(datapoint *);
double euclideanDistance(double *, double *, int);
void printfinalresult(double **, int);
void powDiagonalSquareMatrix(double **, double, int);
double **multiplySquareMatrices(double **, double **, int);
double frobeniusNorm(double **, double **, int, int);
void multiplyNonSquareMatrices(double **, double **, double **, int, int, int);
void transposeMatrix(double **, double **, int, int);
void copyMatrix(double **, double **, int, int);

datapoint *readText(char *fileName)
{
    datapoint *head_vec, *curr_vec, *prev_vec = NULL;
    cord *head_cord, *curr_cord;
    double n;
    char c;
    FILE *ptr = fopen(fileName, "r");
    if (ptr == NULL)
    {
        printf("An Error Has Occurred");
        exit(1);
    }
    datapointsListLen = 0;
    datapointLen = 0;
    head_cord = malloc(sizeof(cord));
    if (head_cord == NULL)
    {
        printf("An Error Has Occurred");
        exit(1);
    }
    curr_cord = head_cord;
    curr_cord->next = NULL;
    head_vec = malloc(sizeof(datapoint));
    if (head_vec == NULL)
    {
        printf("An Error Has Occurred");
        free(head_cord);
        exit(1);
    }
    curr_vec = head_vec;
    curr_vec->next = NULL;
    while (fscanf(ptr, "%lf%c", &n, &c) == 2)
    {
        if (c == '\n')
        {
            datapointsListLen++;
            curr_cord->value = n;
            curr_vec->cords = head_cord;
            curr_vec->next = malloc(sizeof(datapoint));
            if (curr_vec->next == NULL)
            {
                printf("An Error Has Occurred");
                freeLinkedListMemory(head_vec);
                exit(1);
            }
            prev_vec = curr_vec;
            curr_vec = curr_vec->next;
            curr_vec->next = NULL;
            head_cord = malloc(sizeof(cord));
            if (head_cord == NULL)
            {
                printf("An Error Has Occurred");
                freeLinkedListMemory(head_vec);
                exit(1);
            }
            curr_cord = head_cord;
            curr_cord->next = NULL;
            continue;
        }
        curr_cord->value = n;
        curr_cord->next = malloc(sizeof(cord));
        if (curr_cord->next == NULL)
        {
            printf("An Error Has Occurred");
            freeLinkedListMemory(head_vec);
            exit(1);
        }
        curr_cord = curr_cord->next;
        curr_cord->next = NULL;
    }
    prev_vec->next = NULL;
    free(curr_cord);
    free(curr_vec);

    /*counting datapoint len*/
    curr_cord = head_vec->cords;
    while (curr_cord != NULL)
    {
        datapointLen++;
        curr_cord = curr_cord->next;
    }
    fclose(ptr);
    return head_vec;
}

double euclideanDistance(double *x, double *y, int dimension)
{
    int i;
    double distance = 0;
    for (i = 0; i < dimension; i++)
    {
        distance += pow((x[i] - y[i]), 2);
    }
    return distance;
}

/*Calculates and outputs the similarity matrix A */
/*Dosen't allocate memory of A*/
double **sym(double **datapoints, int n, int dimension)
{
    int i, j;
    double eucDistance;
    double **A = (double **)malloc(n * sizeof(double *));
    if (A == NULL)
    {
        printf("An Error Has Occurred");
        return NULL;
    }
    for (i = 0; i < n; i++)
    {
        A[i] = (double *)malloc(n * sizeof(double));
        if (A[i] == NULL)
        {
            printf("An Error Has Occurred");
            freeArrayOfArraysMemory(A, i - 1);
            return NULL;
        }
        for (j = 0; j < n; j++)
        {
            if (i == j)
            {
                A[i][j] = 0;
            }
            else
            {
                eucDistance = euclideanDistance(datapoints[i], datapoints[j], dimension);
                A[i][j] = exp(-1 * eucDistance / 2);
            }
        }
    }
    return A;
}

/*Calculates and outputs the Diagonal Degree Matrix D, using A - the similarity matrix */
/*Doesn't free memory of A, D */
double **ddg(double **A, int n)
{
    int i, j, k;
    double **D = (double **)calloc(n, sizeof(double *));
    if (D == NULL)
    {
        printf("An Error Has Occurred");
        freeArrayOfArraysMemory(A, n);
        return NULL;
    }
    for (i = 0; i < n; i++)
    {
        D[i] = (double *)calloc(n, sizeof(double));
        if (D[i] == NULL)
        {
            printf("An Error Has Occurred");
            freeArrayOfArraysMemory(A, n);
            freeArrayOfArraysMemory(D, i - 1);
            return NULL;
        }
        for (j = 0; j < n; j++)
        {
            if (i == j)
            {
                /*sum the i'th row of A: */
                for (k = 0; k < n; k++)
                {
                    D[i][i] += A[i][k];
                }
            }
            else
            {
                D[i][j] = 0;
            }
        }
    }
    return D;
}

/*Calculates and outputs the normalized similarity matrix W: */
/*Allocates memory of A, D */
/*Dosen't allocate memory of W */
double **norm(double **A, double **D, int n)
{
    double **intermidate;
    double **result = NULL;
    powDiagonalSquareMatrix(D, -0.5, n);
    intermidate = multiplySquareMatrices(D, A, n);
    if (intermidate != NULL)
    {
        result = multiplySquareMatrices(intermidate, D, n);
        freeArrayOfArraysMemory(A, n);
        if (result != NULL)
        {
            freeArrayOfArraysMemory(D, n);
            freeArrayOfArraysMemory(intermidate, n);
        }
    }
    return result;
}

void multiplyNonSquareMatrices(double **A, double **B, double **C, int rowsA, int colsA, int colsB)
{
    int i, j, k;
    for (i = 0; i < rowsA; i++)
    {
        for (j = 0; j < colsB; j++)
        {
            C[i][j] = 0;
            for (k = 0; k < colsA; k++)
            {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }
}

double **multiplySquareMatrices(double **m1, double **m2, int size)
{
    int i, j, k;
    double **result = (double **)malloc(size * sizeof(double *));
    if (result == NULL)
    {
        printf("An Error Has Occurred");
        freeArrayOfArraysMemory(m1, size);
        freeArrayOfArraysMemory(m2, size);
        return NULL;
    }
    for (i = 0; i < size; i++)
    {
        result[i] = (double *)malloc(size * sizeof(double));
        if (result[i] == NULL)
        {
            printf("An Error Has Occurred");
            freeArrayOfArraysMemory(result, i - 1);
            freeArrayOfArraysMemory(m1, size);
            freeArrayOfArraysMemory(m2, size);
            return NULL;
        }
    }
    for (i = 0; i < size; i++)
    {
        for (j = 0; j < size; j++)
        {
            result[i][j] = 0;
            for (k = 0; k < size; k++)
            {
                result[i][j] += m1[i][k] * m2[k][j];
            }
        }
    }
    return result;
}

void powDiagonalSquareMatrix(double **matrix, double number, int size)
{
    int i;
    for (i = 0; i < size; i++)
    {
        matrix[i][i] = pow(matrix[i][i], number);
    }
}

double **symnmf(double **W, double **H, int n, int k, double epsilon, int maxIter)
{
    int i, j;
    double **HNew, **WmultH, **HT, **HmultHT, **HmultHTmultH;    
    int iter = 0;
    /*allocate memory for help matrices*/
    HNew = (double **)malloc(n * sizeof(double *));
    if (HNew == NULL)
    {
        return NULL;
    }
    for (i = 0; i < n; i++)
    {
        HNew[i] = (double *)malloc(k * sizeof(double));
        if (HNew[i] == NULL)
        {
            freeArrayOfArraysMemory(HNew, i - 1);
            return NULL;
        }
    }
    WmultH = (double **)malloc(n * sizeof(double *));
    if (WmultH == NULL)
    {
        freeArrayOfArraysMemory(HNew, n);
        return NULL;
    }
    for (i = 0; i < n; i++)
    {
        WmultH[i] = (double *)malloc(k * sizeof(double));
        if (WmultH[i] == NULL)
        {
            freeArrayOfArraysMemory(HNew, n);
            freeArrayOfArraysMemory(WmultH, i - 1);
            return NULL;
        }
    }
    HT = (double **)malloc(k * sizeof(double *));
    if (HT == NULL)
    {
        freeArrayOfArraysMemory(HNew, n);
        freeArrayOfArraysMemory(WmultH, n);
        return NULL;
    }
    for (i = 0; i < k; i++)
    {
        HT[i] = (double *)malloc(n * sizeof(double));
        if (HT[i] == NULL)
        {
            freeArrayOfArraysMemory(HNew, n);
            freeArrayOfArraysMemory(WmultH, n);
            freeArrayOfArraysMemory(HT, i - 1);
            return NULL;
        }
    }
    HmultHT = (double **)malloc(n * sizeof(double *));
    if (HmultHT == NULL)
    {
        freeArrayOfArraysMemory(HNew, n);
        freeArrayOfArraysMemory(WmultH, n);
        freeArrayOfArraysMemory(HT, k);
        return NULL;
    }
    for (i = 0; i < n; i++)
    {
        HmultHT[i] = (double *)malloc(n * sizeof(double));
        if (HmultHT[i] == NULL)
        {
            freeArrayOfArraysMemory(HNew, n);
            freeArrayOfArraysMemory(WmultH, n);
            freeArrayOfArraysMemory(HT, k);
            freeArrayOfArraysMemory(HmultHT, i - 1);
            return NULL;
        }
    }
    HmultHTmultH = (double **)malloc(n * sizeof(double *));
    if (HmultHTmultH == NULL)
    {
        freeArrayOfArraysMemory(HNew, n);
        freeArrayOfArraysMemory(WmultH, n);
        freeArrayOfArraysMemory(HT, k);
        freeArrayOfArraysMemory(HmultHT, n);
        return NULL;
    }
    for (i = 0; i < n; i++)
    {
        HmultHTmultH[i] = (double *)malloc(k * sizeof(double));
        if (HmultHTmultH[i] == NULL)
        {
            freeArrayOfArraysMemory(HNew, n);
            freeArrayOfArraysMemory(WmultH, n);
            freeArrayOfArraysMemory(HT, k);
            freeArrayOfArraysMemory(HmultHT, n);
            freeArrayOfArraysMemory(HmultHTmultH, i - 1);
            return NULL;
        }
    }
    /*function body*/
    copyMatrix(H, HNew, n, k);
    while (iter <= maxIter)
    {
        multiplyNonSquareMatrices(W, H, WmultH, n, n, k);
        transposeMatrix(H, HT, n, k);
        multiplyNonSquareMatrices(H, HT, HmultHT, n, k, n);
        multiplyNonSquareMatrices(HmultHT, H, HmultHTmultH, n, n, k);
        for (i = 0; i < n; i++)
        {
            for (j = 0; j < k; j++)
            {
                HNew[i][j] = H[i][j] * (1 - 0.5 + 0.5 * (WmultH[i][j] / HmultHTmultH[i][j]));
            }
        }
        if (frobeniusNorm(HNew, H, n, k) < epsilon)
        {
            break;
        }
        iter++;
        copyMatrix(HNew, H, n, k);
    }
    freeArrayOfArraysMemory(WmultH, n);
    freeArrayOfArraysMemory(HT, k);
    freeArrayOfArraysMemory(HmultHT, n);
    freeArrayOfArraysMemory(HmultHTmultH, n);
    return HNew;
}

void copyMatrix(double **from, double **to, int rows, int cols)
{
    int i, j;
    for (i = 0; i < rows; i++)
    {
        for (j = 0; j < cols; j++)
        {
            to[i][j] = from[i][j];
        }
    }
}

void transposeMatrix(double **H, double **HT, int n, int k)
{
    int i, j;
    for (i = 0; i < n; i++)
    {
        for (j = 0; j < k; j++)
        {
            HT[j][i] = H[i][j];
        }
    }
}

double frobeniusNorm(double **H1, double **H2, int rows, int cols)
{
    int i, j;
    double diff;
    double norm = 0;
    for (i = 0; i < rows; i++)
    {
        for (j = 0; j < cols; j++)
        {
            diff = H1[i][j] - H2[i][j];
            norm += pow(diff, 2);
        }
    }
    return norm;
}

/* responsible for freeing the memory of the given datapoints array. doesnt matter if this array is
created in this program or given from the c module (python). */
void freeArrayOfArraysMemory(double **array, int len)
{
    int i = 0;
    for (i = 0; i < len; i++)
    {
        free(array[i]);
    }
    free(array);
}

/* responsible for freeing the memory of the linked list that helped us to read the text file.
this list was converted to array of arrays. */
void freeLinkedListMemory(datapoint *head)
{
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
}

double **convertLinkedListToArrayOfArrays(datapoint *head, int listLen, int dataPointLen)
{
    int i, j;
    datapoint *tmpHead = head;
    double **datapoints = (double **)malloc(listLen * sizeof(double *));
    if (datapoints == NULL)
    {
        printf("An Error Has Occurred");
        freeLinkedListMemory(tmpHead);
        exit(1);
    }
    for (i = 0; i < listLen; i++)
    {
        cord *currCord;
        datapoints[i] = (double *)malloc(dataPointLen * sizeof(double));
        currCord = head->cords;
        if (datapoints[i] == NULL)
        {
            printf("An Error Has Occurred");
            freeArrayOfArraysMemory(datapoints, i - 1);
            freeLinkedListMemory(tmpHead);
            exit(1);
        }
        for (j = 0; j < dataPointLen; j++)
        {
            datapoints[i][j] = currCord->value;
            currCord = currCord->next;
        }
        head = head->next;
    }
    freeLinkedListMemory(tmpHead);
    return datapoints;
}

void printfinalresult(double **matrix, int matrixDimension)
{
    int i, j;
    for (i = 0; i < matrixDimension; i++)
    {
        for (j = 0; j < matrixDimension; j++)
        {
            if (j == matrixDimension - 1)
            {
                printf("%.4f\n", matrix[i][j]);
            }
            else
            {
                printf("%.4f,", matrix[i][j]);
            }
        }
    }
}

int main(int argc, char *argv[])
{
    char *goal;
    char *fileName;
    datapoint *head;
    double **A, **D, **datapoints, **result;
    if (argc == 3)
    {
        goal = argv[1];
        fileName = argv[2];
        head = readText(fileName);
        datapoints = convertLinkedListToArrayOfArrays(head, datapointsListLen, datapointLen);
        if (!strcmp(goal, "sym"))
        {
            /* Calculate similarity matrix A: */
            result = sym(datapoints, datapointsListLen, datapointLen);
            if (result != NULL)
            {
                printfinalresult(result, datapointsListLen);
                freeArrayOfArraysMemory(result, datapointsListLen);
            }
        }
        else if (!strcmp(goal, "ddg"))
        {
            /* Calculate similarity matrix A: */
            A = sym(datapoints, datapointsListLen, datapointLen);
            if(A != NULL) {
                result = ddg(A, datapointsListLen);
                /*Calculate diagonal degree Matrix D:*/
                if (result != NULL)
                {
                    printfinalresult(result, datapointsListLen);
                    freeArrayOfArraysMemory(result, datapointsListLen);
                    freeArrayOfArraysMemory(A, datapointsListLen);
                }
            }
        }
        else if (!strcmp(goal, "norm"))
        {
            A = sym(datapoints, datapointsListLen, datapointLen);
            if(A != NULL) {
                D = ddg(A, datapointsListLen);
                if(D != NULL) {
                    result = norm(A, D, datapointsListLen);
                    if (result != NULL) {
                        printfinalresult(result, datapointsListLen);
                        freeArrayOfArraysMemory(result, datapointsListLen);
                        /*freeing memory of A, D happens in function norm*/
                    }
                }
            }
        }
        /* free datapoints memory at the end of the program*/
        freeArrayOfArraysMemory(datapoints, datapointsListLen);
    }
    return 0;
}
