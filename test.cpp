#include <CL/cl.h>
#include <stdio.h>

int main() {
    cl_int err;
    cl_uint num_platforms;
    err = clGetPlatformIDs(0, NULL, &num_platforms);
    if (err != CL_SUCCESS) {
        printf("Error clGetPlatformIDs: %d\n", err);
        return -1;
    }
    if (num_platforms == 0) {
        printf("OpenCL platforms not found\n");
        return -1;
    }

    cl_platform_id platform;
    clGetPlatformIDs(1, &platform, NULL);

    cl_device_id device;
    err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &device, NULL);
    if (err != CL_SUCCESS) {
        printf("Error clGetDeviceIDs: %d\n", err);
        return -1;
    }

    cl_context context = clCreateContext(NULL, 1, &device, NULL, NULL, &err);
    if (err != CL_SUCCESS) {
        printf("Error clCreateContext: %d\n", err);
        return -1;
    }

    printf("Context successfully created\n");
    clReleaseContext(context);
    return 0;
}