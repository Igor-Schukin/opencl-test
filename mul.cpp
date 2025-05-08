#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "opencl.h"

const char* CL_KERNEL_SOURCE = "mul.cl";
const char* CL_KERNEL_NAME = "mul";

const size_t DIM  = 100;       // 2D square matrix dimension
const size_t SIZE = DIM * DIM; // 1D array size for square matrix
#define ID(r, c) ((r)*DIM+(c)) // 1D index for 2D matrix

void printMatrix(int* matrix)
{
    for (size_t i = 0; i < DIM; i++)
    {
        for (size_t j = 0; j < DIM; j++)
        {
            printf("%4d ", matrix[ID(i,j)]);
            if (j >= 9) { printf(" ...."); break; }
        }
        printf("\n");
        if (i >= 9) { printf("....\n"); break; }
    }
}

int main()
{
    srand(time(NULL));

    size_t tsStart, tsEnd;

    // Input data

    int a[SIZE], b[SIZE], result[SIZE];
    for (int i = 0; i < SIZE; i++)
    {
        a[i] = rand() % 201 - 100; 
        b[i] = rand() % 201 - 100;
    }

    printf("\n~~~~~ Let's go with OpenCL\n");

    OpenCL job(CL_KERNEL_SOURCE, CL_KERNEL_NAME);

    tsStart = getTime();
    job.run(
        {
            {ArgTypes::IN_IBUF,  (void*)a,      SIZE },
            {ArgTypes::IN_IBUF,  (void*)b,      SIZE },
            {ArgTypes::OUT_IBUF, (void*)result, SIZE },
            {ArgTypes::INT,      (void*)&SIZE,  1    }
        },
        DIM,
        3
    );
    tsEnd = getTime();
    size_t tsWopenCL = tsEnd - tsStart;

    printf("OpenCL result:\n");
    printMatrix(result);

    printf("\n~~~~~ Let's go without OpenCL\n");

    int refResult[SIZE]{};

    tsStart = getTime();
    for (size_t r = 0; r < DIM; r++)
        for (size_t c = 0; c < DIM; c++)
            for (size_t k = 0; k < DIM; k++)
                refResult[ID(r,c)] += a[ID(r,k)] * b[ID(k,c)];

    tsEnd = getTime();
    size_t tsWOopenCL = tsEnd - tsStart;

    printf("CPU result:\n");
    printMatrix(refResult);

    bool isEqual = true;
    for (size_t i = 0; i < SIZE; i++)
    {
        if (result[i] != refResult[i])
        {
            isEqual = false;
            break;
        }
    }

    printf("\n~~~~~ Results comparison\n");
    if (isEqual) printf("   OpenCL and CPU result are the same\n");
    else printf("   OpenCL and CPU results differ!\n");

    printf("\n~~~~~ Execution time\n");
    printf("     with OpenCL: %zu ms\n", tsWopenCL);
    printf("  without OpenCL: %zu ms\n", tsWOopenCL);
    printf("\n~~~~~ Bye!\n");

    return 0;
}