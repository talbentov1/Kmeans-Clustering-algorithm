struct cord
{
    double value;
    struct cord *next;
};
struct datapoint
{
    struct datapoint *next;
    struct cord *cords;
};

int datapointsListLen;
int datapointLen;

typedef struct datapoint datapoint;
typedef struct cord cord;

double **sym(double **, int, int);
double **ddg(double **, int);
double **norm(double **, double **, int);
void freeArrayOfArraysMemory(double **, int);
double **symnmf(double **, double **, int, int, double, int);
