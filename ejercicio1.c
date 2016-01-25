#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

void Usage(char prog_name[]);



//void Get_args(void *p);
void Gen_data(void *p);
void Gen_bins(void *p);
int Which_bin(void *p);
void Print_histo(void *p);


void func(void *p);

struct test
{
    int bin_count, i, bin;
    float min_meas, max_meas;
    float* bin_maxes;
    int* bin_counts;
    int data_count;
    float* data;
};

typedef struct test struct1;

int main(int argc, char* argv[])
{

    struct1 *p;
    pthread_t th1, th2, th3;

    if (argc != 5)
        Usage(argv[0]);
    p->bin_count = strtol(argv[1], NULL, 10);
    p->min_meas = strtof(argv[2], NULL);
    p->max_meas = strtof(argv[3], NULL);
    p->data_count = strtol(argv[4], NULL, 10);
	
    
    p->bin_maxes = malloc(p->bin_count*sizeof(float));
    p->bin_counts = malloc(p->bin_count*sizeof(int));
    p->data = malloc(p->data_count*sizeof(float));


    pthread_create(&th1,NULL,(void*) Gen_data,(void*) p);
    pthread_create(&th2,NULL,(void*) Gen_bins,(void*) p);
    pthread_create(&th3,NULL,(void*) func,(void*) p);



    pthread_join(th1,NULL);
    pthread_join(th2,NULL);
    pthread_join(th3,NULL);


    Print_histo(p);
    free(p->data);
    free(p->bin_maxes);
    free(p->bin_counts);

    return 0;
}  /* main */

void func(void *p)
{
    int i;
    struct1 *args;
    args=(struct1*)p;

    for (i = 0; i < args->data_count; i++)
    {
        args->bin = Which_bin(args);
        args->bin_counts[args->bin]++;
    }

}

/*---------------------------------------------------------------------
 * Function:  Usage
 * Purpose:   Print a message showing how to run program and quit
 * In arg:    prog_name:  the name of the program from the command line
 */
void Usage(char prog_name[] /* in */)
{
    fprintf(stderr, "usage: %s ", prog_name);
    fprintf(stderr, "<bin_count> <min_meas> <max_meas> <data_count>\n");
    exit(0);
}  /* Usage */


void Gen_data(void *p)
{
    struct1 *args;
    args=(struct1*)p;
    int i;
    srandom(0);
    for (i = 0; i < args->data_count; i++)
        args->data[i] = args->min_meas + (args->max_meas - args->min_meas)*random()/((double) RAND_MAX);

} /* Gen_data */


void Gen_bins(void* p)
{
    struct1 *args;
    args=(struct1*)p;
    float bin_width;
    int   i;
    bin_width = (args->max_meas - args->min_meas)/args->bin_count;

    for (i = 0; i < args->bin_count; i++)
    {
        args->bin_maxes[i] = args->min_meas + (i+1)*bin_width;
        args->bin_counts[i] = 0;
    }


}

int Which_bin(void* p)
{
    struct1 *args;
    args=(struct1*)p;
    int bottom = 0, top =  args->bin_count-1;
    int mid;
    float bin_max, bin_min;

    while (bottom <= top)
    {
        mid = (bottom + top)/2;
        bin_max = args->bin_maxes[mid];
        bin_min = (mid == 0) ? args->min_meas: args->bin_maxes[mid-1];
        if (*(args->data) >= bin_max)
            bottom = mid+1;
        else if (*(args->data) < bin_min)
            top = mid-1;
        else
            return mid;
    }
    fprintf(stderr, "Data = %f doesn't belong to a bin!\n", args->data);
    fprintf(stderr, "Quitting\n");
    exit(-1);
}

void Print_histo(void *p)
{
    struct1 *args;
    args=(struct1*)p;
    int i, j;
    float bin_max, bin_min;

    for (i = 0; i < args->bin_count; i++)
    {
        bin_max = args->bin_maxes[i];
        bin_min = (i == 0) ? args->min_meas: args->bin_maxes[i-1];
        printf("%.3f-%.3f:\t", bin_min, bin_max);
        for (j = 0; j < args->bin_counts[i]; j++)
            printf("X");
        printf("\n");
    }
}

/* Print_histo */
