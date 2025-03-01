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

int k;
int iter;
int datapointsListLen;
int datapointLen;
double **centroids;
double **new_centroids;
const double EPSILON = 0.001;

datapoint *readText();
void validateInput(datapoint *, int);
void initialize(datapoint *);
void assignClusters(datapoint *);
double euclideanDistance(cord *, double *);
double euclideanDistanceArr(double *, double *);
void updateCentroids(datapoint *);
int deltaCentroidCheck();
void printfinalresult();
void freeMemory(datapoint *, int, int);
void swapCentroids(double ***, double ***);

datapoint *readText()
{
    datapoint *head_vec, *curr_vec, *prev_vec;
    cord *head_cord, *curr_cord;
    double n;
    char c;
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
    while (scanf("%lf%c", &n, &c) == 2)
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
                freeMemory(head_vec, 0, 0);
                exit(1);
            }
            prev_vec = curr_vec;
            curr_vec = curr_vec->next;
            curr_vec->next = NULL;
            head_cord = malloc(sizeof(cord));
            if (head_cord == NULL)
            {
                printf("An Error Has Occurred");
                freeMemory(head_vec, 0, 0);
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
            freeMemory(head_vec, 0, 0);
            exit(1);
        }
        curr_cord = curr_cord->next;
        curr_cord->next = NULL;
    }
    prev_vec->next = NULL;
    free(curr_cord);
    free(curr_vec);

    validateInput(head_vec, datapointsListLen);

    /*counting datapoint len*/
    curr_cord = head_vec->cords;
    while (curr_cord != NULL)
    {
        datapointLen++;
        curr_cord = curr_cord->next;
    }

    return head_vec;
}

void validateInput(datapoint *head, int N)
{
    if ((k <= 1 || k >= N) || (iter <= 1 || iter >= 1000))
    {
        if (k <= 1 || k >= N)
        {
            printf("Invalid number of clusters!\n");
        }
        if (iter <= 1 || iter >= 1000)
        {
            printf("Invalid maximum iteration!\n");
        }
        freeMemory(head, 0, 0);
        exit(1);
    }
}

/* Initializing the first k datapoints to be the centroids and allocating memory for centroids + new_centroids 2-dimensional arrays */
void initialize(datapoint *head)
{
    int i = 0, j = 0;
    cord *curr_cord = head->cords;
    centroids = calloc(k, sizeof(double *));
    if (centroids == NULL)
    {
        printf("An Error Has Occurred");
        freeMemory(head, 0, 0);
        exit(1);
    }
    new_centroids = calloc(k, sizeof(double *));
    if (new_centroids == NULL)
    {
        printf("An Error Has Occurred");
        freeMemory(head, 0, 0);
        exit(1);
    }
    for (; i < k; i++)
    {
        j = 0;
        *(centroids + i) = calloc(datapointLen, sizeof(double));
        if (centroids[i] == NULL)
        {
            printf("An Error Has Occurred");
            freeMemory(head, i - 1, i - 1);
            exit(1);
        }
        new_centroids[i] = calloc(datapointLen, sizeof(double));
        if (new_centroids[i] == NULL)
        {
            printf("An Error Has Occurred");
            freeMemory(head, i, i - 1);
            exit(1);
        }
        for (; j < datapointLen; j++)
        {
            centroids[i][j] = curr_cord->value;
            curr_cord = curr_cord->next;
        }
        head = head->next;
        curr_cord = head->cords;
    }
}

void assignClusters(datapoint *head)
{
    int min_cluster;
    double min_distance, curr_distance;
    int i;
    while (head != NULL)
    {
        min_distance = euclideanDistance(head->cords, centroids[0]);
        min_cluster = 0;
        for (i = 1; i < k; i++)
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

double euclideanDistanceArr(double *arr1, double *arr2)
{
    double sum = 0;
    int i;
    for (i = 0; i < datapointLen; i++)
    {
        sum += pow((arr1[i] - arr2[i]), 2);
    }
    return sqrt(sum);
}

void updateCentroids(datapoint *head)
{
    int i, j, t;
    int points_counter = 0;
    datapoint *curr = head;
    cord *tmp;
    double *sum_coordinates = calloc(datapointLen, sizeof(double));
    if (sum_coordinates == NULL)
    {
        printf("An Error Has Occurred");
        freeMemory(head, k, k);
        exit(1);
    }
    /*for every centroid: */
    for (i = 0; i < k; i++)
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
}

int deltaCentroidCheck()
{
    int i;
    /*for every centroid: */
    for (i = 0; i < k; i++)
    {
        if (euclideanDistanceArr(centroids[i], new_centroids[i]) > EPSILON)
        {
            return 0;
        }
    }
    return 1;
}

void printfinalresult()
{
    int i, j;
    for (i = 0; i < k; i++)
    {
        for (j = 0; j < datapointLen; j++)
        {
            if (j == datapointLen - 1)
            {
                printf("%.4f\n", centroids[i][j]);
            }
            else
            {
                printf("%.4f,", centroids[i][j]);
            }
        }
    }
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
    /*free centroids: */
    for (i = 0; i < centroids_size; i++)
    {
        free(centroids[i]);
    }
    free(centroids);
    /*free new_centroids: */
    for (i = 0; i < new_centroids_size; i++)
    {
        free(new_centroids[i]);
    }
    free(new_centroids);
}

void swapCentroids(double ***centroids1, double ***centroids2)
{
    double **temp = *centroids1;
    *centroids1 = *centroids2;
    *centroids2 = temp;
}

int main(int argc, char *argv[])
{
    int iter_num = 0;
    datapoint *head;
    if (argc == 3)
    {
        k = atoi(argv[1]);
        iter = atoi(argv[2]);
    }
    if (argc == 2)
    {
        k = atoi(argv[1]);
        iter = 200;
    }
    /*read file*/
    head = readText();

    /*initialize k centroids*/
    initialize(head);

    while (iter_num < iter)
    {
        assignClusters(head);
        updateCentroids(head);
        if (deltaCentroidCheck() == 1)
        {
            break;
        }
        swapCentroids(&centroids, &new_centroids);
        iter_num++;
    }
    printfinalresult();
    freeMemory(head, k, k);
    return 0;
}
