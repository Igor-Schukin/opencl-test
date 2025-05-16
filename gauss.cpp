#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "opencl.h"

const char* CL_KERNEL_SOURCE = "gauss.cl";
const char* CL_KERNEL_NAME = "toTriangular";

const size_t DIM  = 100;               // 2D square matrix dimension
const size_t SIZE = DIM * (DIM + 1);   // 1D array size for square matrix
#define ID(r, c) ((r)*(DIM+1)+(c))     // 1D index for 2D matrix

void printMatrix(float* matrix)
{
    for (size_t i = 0; i < DIM; i++)
    {
        if (i >= 10) { printf("....\n"); break; }
        for (size_t j = 0; j < DIM + 1; j++)
        {
            if (j >= 10) { printf(" ...."); break; }
            printf("%10.2f ", matrix[ID(i,j)]);
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

        float *m = new float[SIZE], *result = new float[DIM];
        for (int i = 0; i < SIZE; i++)
        {
            m[i] = (rand() % 2001 - 1000) / 100.0f;
        }

        printf("\n~~~~~ Let's go with OpenCL\n");

        OpenCL job(CL_KERNEL_SOURCE, CL_KERNEL_NAME);

        tsStart = getTime();
        job.run(
            {
                {ArgTypes::IN_OUT_FBUF,  (void*)m,      SIZE },
                {ArgTypes::OUT_FBUF,     (void*)result, DIM  },
                {ArgTypes::INT,          (void*)&DIM,   1    }
            },
            { DIM, DIM, DIM+1 },
            { 1, 1, DIM+1 }
            3
        );
        tsEnd = getTime();
        size_t tsWopenCL = tsEnd - tsStart;

        printMatrix(result);
/*
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
*/
        delete [] m;
        delete [] result;

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