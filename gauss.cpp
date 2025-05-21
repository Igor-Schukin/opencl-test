#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <cmath>
#include "opencl.h"

const char* CL_KERNEL_SOURCE = "gauss.cl";
const char* CL_KERNEL_FW    = "zeroOutCol";
const char* CL_KERNEL_BW    = "calcRoot";
const char* CL_KERNEL_CHECK = "calcError";

const size_t DIM  = 3;              // 2D square matrix dimension
const size_t SIZE = DIM * (DIM + 1);   // 1D array size for 2D extended matrix
#define ID(r, c) ((r)*(DIM+1)+(c))     // 1D index for 2D extended matrix

/*
void printMatrix(float* matrix)
{
    for (size_t i = 0; i < DIM; i++)
    {
        if (i >= 10) { printf("....\n"); break; }
        for (size_t j = 0; j < DIM + 1; j++)
        {
            if (j >= 10) { printf(" ...."); break; }
            float f = matrix[ID(i,j)];
            if (fabs(f) > 1E5) printf("%cINF", f < 0 ? '-' : '+');
            else printf("%10.2f ", f);
        }
        printf("\n");
    }
}
*/

void printVector(float* vector)
{
    for (size_t i = 0; i < DIM; i++)
    {
        if (i >= 10) { printf("...."); break; }
        float f = vector[i];
        if (fabs(f) > 1E5) printf("%7cINF", f < 0 ? '-' : '+');
        else printf("%10.2f ", f);
    }
    printf("\n");
}

int main()
{
    try { 

        srand(time(NULL));

        size_t tsStart, tsEnd;

        // Input data

        float *m = new float[SIZE]{1, 5, -1, 4, 8, -9, 2, -10, 3, 5, 11, -8}, *result = new float[DIM], *errors = new float[DIM];

        // float *m = new float[SIZE], *result = new float[DIM], *errors = new float[DIM];
        // for (int i = 0; i < SIZE; i++) m[i] = (rand() % 2001 - 1000) / 100.0f;

        printf("\n~~~~~ Let's go with OpenCL\n");

        OpenCL job(CL_KERNEL_SOURCE, std::vector<std::string>{ CL_KERNEL_FW, CL_KERNEL_BW, CL_KERNEL_CHECK });

        tsStart = getTime();
        int col = 0;
        auto args = std::vector<std::tuple<ArgTypes, void*, size_t>>{
            {ArgTypes::IN_FBUF,      (void*)m,      SIZE },
            {ArgTypes::OUT_FBUF,     (void*)result, DIM  },
            {ArgTypes::OUT_FBUF,     (void*)errors, DIM  },
            {ArgTypes::INT,          (void*)&col,   1    }
        };
        job.createBuffers(args);
        for (col = 0; col < DIM; col++) job.runKernel(0, args, { DIM, DIM+1 }, { 1, DIM+1 });   // forward elimination
        for (col = DIM-1; col >= 0; col--) job.runKernel(1, args, { DIM }, { DIM } );           // backward substitution
        for (col = 0; col < DIM; col++) job.runKernel(2, args, { DIM }, { DIM } );              // check errors
        job.readBuffers(args);

        tsEnd = getTime();
        size_t tsWopenCL = tsEnd - tsStart;

        float err = fabs(errors[0]);
        for (size_t i = 1; i < DIM; i++) if (fabs(errors[i]) > err) err = fabs(errors[i]);

        printVector(result);
        printf("\nError: %f\n", err);

        printf("\n~~~~~ Let's go without OpenCL\n");

        //~~~ matrix copy for final test

        float *mc = new float[SIZE]
        for(int i = 0; i < SIZE; i++) mc[i] = m[i];
        
        tsStart = getTime();

        //~~~ transform matrix to a triangular form
        
        for(int i = 0; i < DIM; i++)
        {
            for(int j = i + 1; j < DIM; j++)
            {
                float ratio = m[ID(j,i)] / m[ID(i,i)];
                for(int k = i + 1; k <= DIM; k++) m[ID(j, k)] = m[ID(j, k)] - ratio * m[ID(i,k)];
            }
        }

        //~~~ calculate roots
        
        for(int i = DIM-1; i >= 0; i--)
        {
            result[i] = m[ID(i,DIM)];
            for(int j = i+1; j < DIM; j++) result[i] -= m[ID(i,j)] * result[j];
            result[i] = result[i] / m[ID(i,i)];
        }
        
        //~~~ calculate error

        err = 0;
        for(int i = 0; i < DIM; i++) {
            float sum = 0;
            for (int j = 0; j < DIM; j++) sum += result[j] * mc[ID(i,j)];
            err = std::max<float>(err, fabs(mc[ID(i,DIM)] - sum));
        }

        tsEnd = getTime();
        size_t tsWopenCL = tsEnd - tsStart;

        printVector(result);
        printf("\nError: %f\n", err);

        delete [] m;
        delete [] mc;
        delete [] result;
        delete [] errors;

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