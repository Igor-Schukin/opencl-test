#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "opencl.h"

const char* CL_KERNEL_SOURCE = "mul.cl";
const char* CL_KERNEL_NAME = "mul";

const size_t DIM  = 2500;      // 2D square matrix dimension
const size_t SIZE = DIM * DIM; // 1D array size for square matrix
#define ID(r, c) ((r)*DIM+(c)) // 1D index for 2D matrix

void printMatrix(int* matrix)
{
    for (size_t i = 0; i < DIM; i++)
    {
        if (i >= 10) { printf("....\n"); break; }
        for (size_t j = 0; j < DIM; j++)
        {
            if (j >= 10) { printf(" ...."); break; }
            printf("%8d ", matrix[ID(i,j)]);
        }
        printf("\n");
    }
}

int main()
{
    try { 

        srand(time(NULL));

        size_t tsStart, tsEnd;

        // Input data

        int *a = new int[SIZE], *b = new int[SIZE], *result = new int[SIZE];
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
                {ArgTypes::INT,      (void*)&DIM,   1    }
            },
            { DIM, DIM, DIM }
        );
        tsEnd = getTime();
        size_t tsWopenCL = tsEnd - tsStart;

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

        delete[] a;
        delete[] b;
        delete[] result;

        printf("\n~~~~~ Results comparison\n");
        if (isEqual) printf("   OpenCL and CPU result are the same\n");
        else printf("   OpenCL and CPU results differ!\n");

        printf("\n~~~~~ Execution time\n");
        printf("     with OpenCL: %zu ms\n", tsWopenCL);
        printf("  without OpenCL: %zu ms\n", tsWOopenCL);
        printf("\n~~~~~ Bye!\n");
    }
    catch (const OpenClError& e)
    {
        printf("OpenCL error: %s\n", e.what());
        return 1;
    }
    catch (const std::exception& e)
    {
        printf("Error: %s\n", e.what());
        return 1;
    }
    catch (...)
    {
        printf("Unknown error\n");
        return 1;
    }
    return 0;
}