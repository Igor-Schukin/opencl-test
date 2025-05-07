#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

const size_t SIZE = 100'000'000; // Array size

size_t getTime()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (size_t)tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

int main()
{
    printf("Let's go with CPU!\n");

    // Input data
    int a[SIZE], b[SIZE], result[SIZE];
    for (int i = 0; i < SIZE; i++) { a[i] = 2 * i; b[i] = -i; }

    size_t tsStart = getTime();
    for (int i = 0; i < SIZE; i++)
    {
        result[i] = a[i] + b[i]; // CPU computation for comparison
    }   
    size_t tsEnd = getTime();

    // Print first 10 results
    printf("First 10 results:\n");
    for (int i = 0; i < 10 && i < SIZE; i++)
    {
        printf("result[%d] = %d\n", i, result[i]);
    }

    printf("Loop execution time: %zu ms\nBye!\n", tsEnd - tsStart);
    return 0;
}

