#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <CL/cl.h>

#define SIZE 100000000 // Array size
#define CL_KERNEL_SOURCE "clsum.cl"

size_t getTime()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (size_t)tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

// Check for OpenCL errors
void checkError(cl_int err, const char *operation)
{
    if (err != CL_SUCCESS)
    {
        fprintf(stderr, "Error during %s: %d\n", operation, err);
        exit(1);
    }
}

// Load OpenCL kernel source code from file
char* loadKernelSource(const char *filename)
{
    FILE *file = fopen(filename, "r");
    if (!file)
    {
        fprintf(stderr, "Failed to load kernel file.\n");
        exit(1);
    }
    fseek(file, 0, SEEK_END);
    size_t file_size = ftell(file);
    rewind(file);

    char *kernelSource = (char *)malloc(file_size + 1);
    if (!kernelSource)
    {
        fprintf(stderr, "Failed to allocate memory for kernel source.\n");
        fclose(file);
        exit(1);
    }
    fread(kernelSource, 1, file_size, file);
    kernelSource[file_size] = '\0'; // Null-terminate the string
    fclose(file);

    return kernelSource;
}

int main()
{
    printf("Let's go with GPU!\n");

    // Input data
    int a[SIZE], b[SIZE], result[SIZE];
    for (int i = 0; i < SIZE; i++)
    {
        a[i] = i;        // First array: 0, 1, 2, ...
        b[i] = SIZE - i; // Second array: 1024, 1023, 1022, ...
    }

    // Initialize OpenCL
    cl_int err;
    cl_platform_id platform;
    cl_device_id device;
    cl_context context;
    cl_command_queue queue;
    cl_program program;
    cl_kernel kernel;

    // Get platform
    err = clGetPlatformIDs(1, &platform, NULL);
    checkError(err, "clGetPlatformIDs");

    // Get device
    err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &device, NULL);
    checkError(err, "clGetDeviceIDs");

    // Create context
    context = clCreateContext(NULL, 1, &device, NULL, NULL, &err);
    checkError(err, "clCreateContext");

    // Create command queue
    queue = clCreateCommandQueueWithProperties(context, device, NULL, &err);
    checkError(err, "clCreateCommandQueueWithProperties");

    // Create program
    char* kernelSource = loadKernelSource(CL_KERNEL_SOURCE);
    program = clCreateProgramWithSource(context, 1, (const char**)&kernelSource, NULL, &err);
    checkError(err, "clCreateProgramWithSource");

    // Build program
    err = clBuildProgram(program, 1, &device, NULL, NULL, NULL);
    free(kernelSource);
    if (err != CL_SUCCESS)
    {
        size_t log_size;
        clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, 0, NULL, &log_size);
        char *log = (char *)malloc(log_size);
        clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, log_size, log, NULL);
        fprintf(stderr, "Build error: %s\n", log);
        free(log);
        exit(1);
    }

    // Create kernel
    kernel = clCreateKernel(program, "array_add", &err);
    checkError(err, "clCreateKernel");

    // Create buffers
    cl_mem buffer_a = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(int) * SIZE, a, &err);
    checkError(err, "clCreateBuffer a");
    cl_mem buffer_b = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(int) * SIZE, b, &err);
    checkError(err, "clCreateBuffer b");
    cl_mem buffer_result = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(int) * SIZE, NULL, &err); 
    checkError(err, "clCreateBuffer result");

    // Set kernel arguments
    err = clSetKernelArg(kernel, 0, sizeof(cl_mem), &buffer_a);
    checkError(err, "clSetKernelArg 0");
    err = clSetKernelArg(kernel, 1, sizeof(cl_mem), &buffer_b);
    checkError(err, "clSetKernelArg 1");
    err = clSetKernelArg(kernel, 2, sizeof(cl_mem), &buffer_result);
    checkError(err, "clSetKernelArg 2");
    size_t size = SIZE;
    err = clSetKernelArg(kernel, 3, sizeof(int), &size);
    checkError(err, "clSetKernelArg 3");

    // Execute kernel
    size_t tsStart = getTime();
    size_t global_size = SIZE;
    err = clEnqueueNDRangeKernel(queue, kernel, 1, NULL, &global_size, NULL, 0, NULL, NULL);
    checkError(err, "clEnqueueNDRangeKernel");
    size_t tsEnd = getTime();
    printf("Kernel execution time: %zu ms\n", tsEnd - tsStart);

    // Read result
    err = clEnqueueReadBuffer(queue, buffer_result, CL_TRUE, 0, sizeof(int) * SIZE, result, 0, NULL, NULL);
    checkError(err, "clEnqueueReadBuffer");

    // Print first 10 results
    printf("First 10 results:\n");
    for (int i = 0; i < 10 && i < SIZE; i++)
    {
        printf("result[%d] = %d\n", i, result[i]);
    }

    // Clean up resources
    clReleaseMemObject(buffer_a);
    clReleaseMemObject(buffer_b);
    clReleaseMemObject(buffer_result);
    clReleaseKernel(kernel);
    clReleaseProgram(program);
    clReleaseCommandQueue(queue);
    clReleaseContext(context);

    printf("Bye!\n");
    return 0;
}