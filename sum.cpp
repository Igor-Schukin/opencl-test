#include <stdio.h>
#include <stdlib.h>
#include "opencl.h"

const char* CL_KERNEL_SOURCE = "sum.cl";
const char* CL_KERNEL_NAME = "sum";

const size_t SIZE = 100'000'000; // Array size

int main()
{
    size_t tsStart, tsEnd;

    // Input data
    int a[SIZE], b[SIZE], result[SIZE];
    for (int i = 0; i < SIZE; i++) { a[i] = 2 * i; b[i] = -i; }

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
        SIZE
    );
    tsEnd = getTime();
    size_t tsWopenCL = tsEnd - tsStart;

    printf("First 10 results:\n");
    for (int i = 0; i < 10 && i < SIZE; i++) printf("result[%d] = %d\n", i, result[i]);

    printf("\n~~~~~ Let's go without OpenCL\n");

    tsStart = getTime();
    for (int i = 0; i < SIZE; i++)
    {
        result[i] = a[i] + b[i]; 
    }   
    tsEnd = getTime();
    size_t tsWOopenCL = tsEnd - tsStart;

    printf("First 10 results:\n");
    for (int i = 0; i < 10 && i < SIZE; i++)
    {
        printf("result[%d] = %d\n", i, result[i]);
    }

    printf("\n~~~~~ Execution time\n");
    printf("     with OpenCL: %zu ms\n", tsWopenCL);
    printf("  without OpenCL: %zu ms\n", tsWOopenCL);
    printf("\n~~~~~ Bye!\n");

    return 0;
}