#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include "opencl.h"

const char* CL_KERNEL_SOURCE = "clsum.cl";
const char* CL_KERNEL_NAME = "array_add";

const size_t SIZE = 100'000'000; // Array size

size_t getTime()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (size_t)tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

int main()
{
    printf("Let's go with GPU!\n");

    // Input data
    int a[SIZE], b[SIZE], result[SIZE];
    for (int i = 0; i < SIZE; i++) { a[i] = 2 * i; b[i] = -i; }

    OpenCL job(CL_KERNEL_SOURCE, CL_KERNEL_NAME);

    size_t tsStart = getTime();
    job.run(
        SIZE,
        {
            {ArgTypes::IN_IBUF,  (void*)a,      SIZE },
            {ArgTypes::IN_IBUF,  (void*)b,      SIZE },
            {ArgTypes::OUT_IBUF, (void*)result, SIZE },
            {ArgTypes::INT,      (void*)&SIZE,  1    }
        }
    );
    size_t tsEnd = getTime();

    // Print first 10 results
    printf("First 10 results:\n");
    for (int i = 0; i < 10 && i < SIZE; i++) printf("result[%d] = %d\n", i, result[i]);

    printf("Kernel execution time: %zu ms\nBye!\n", tsEnd - tsStart);
    return 0;
}