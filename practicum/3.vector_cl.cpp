#include <iostream>
#include <string>
#include <CL/cl.h>

// Check OpenCL and get the first available device ID

cl_device_id clInit() {
    cl_int error;
    cl_uint numPlatforms;
    cl_platform_id* platforms = nullptr;
    cl_device_id device = nullptr;

    try {
        // Get platform count
        error = clGetPlatformIDs(0, NULL, &numPlatforms);
        if (error != CL_SUCCESS || numPlatforms == 0) throw std::runtime_error("no platforms found or error occurred");

        // Get all platforms
        platforms = new cl_platform_id[numPlatforms];
        error = clGetPlatformIDs(numPlatforms, platforms, NULL);
        if (error != CL_SUCCESS) throw std::runtime_error("failed to get platform IDs");

        cl_uint numDevices;
        cl_device_id* devices = nullptr;
        try {
            // Get device count for the first platform
            error = clGetDeviceIDs(platforms[0], CL_DEVICE_TYPE_ALL, 0, NULL, &numDevices);
            if (error != CL_SUCCESS || numDevices == 0) throw std::runtime_error("failed to get device count or no devices found");

            // Get all device IDs for the first platform
            devices = new cl_device_id[numDevices];
            error = clGetDeviceIDs(platforms[0], CL_DEVICE_TYPE_ALL, numDevices, devices, NULL);
            if (error != CL_SUCCESS) throw std::runtime_error("failed to get device IDs");

            // Select the first device
            device = devices[0];
        }  catch (...) {
            if (devices) delete[] devices;
            throw;
        }

        delete[] devices;
    } catch (...) {
        if (platforms) delete[] platforms;
        throw;
    }

    delete[] platforms;
    return device;
}

const char* vectorAddKernelSource = R"(
    __kernel void vector_add
    (
        __global const float* A, 
        __global const float* B, 
        __global float* C
    ) 
    {
        int id = get_global_id(0);
        C[id] = A[id] + B[id];
    }
)";

int main() {

    // Create float arrays A, B, C and initialize A and B with random values in range 0..10
    const int arraySize = 1024;
    float* arrayA = new float[arraySize];
    float* arrayB = new float[arraySize];
    float* arrayC = new float[arraySize];
    for (int i = 0; i < arraySize; i++) {
        arrayA[i] = static_cast<float>(rand() % 11);
        arrayB[i] = static_cast<float>(rand() % 11);
    }

    try {
        cl_device_id device = clInit();
        cl_int error;
        cl_context context = nullptr;
        cl_command_queue queue = nullptr;
        cl_program program = nullptr;
        cl_kernel kernel = nullptr;
        cl_mem bufferA = nullptr;
        cl_mem bufferB = nullptr;
        cl_mem bufferC = nullptr;

        // Prepare OpenCL context, command queue, program and kernel
        context = clCreateContext(NULL, 1, &device, NULL, NULL, &error);
        if (error != CL_SUCCESS) throw std::runtime_error("failed to create OpenCL context");

        queue = clCreateCommandQueueWithProperties(context, device, NULL, &error);
        if (error != CL_SUCCESS) throw std::runtime_error("failed to create command queue");

        program = clCreateProgramWithSource(context, 1, &vectorAddKernelSource, NULL, &error);
        if (error != CL_SUCCESS) throw std::runtime_error("failed to create program");

        error = clBuildProgram(program, 1, &device, NULL, NULL, NULL);
        if (error != CL_SUCCESS) throw std::runtime_error("failed to build program");

        kernel = clCreateKernel(program, "vector_add", &error);
        if (error != CL_SUCCESS) throw std::runtime_error("failed to create kernel");

        // Create buffers
        bufferA = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(float) * arraySize, arrayA, &error);
        if (error != CL_SUCCESS) throw std::runtime_error("failed to create buffer A");

        bufferB = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(float) * arraySize, arrayB, &error);
        if (error != CL_SUCCESS) throw std::runtime_error("failed to create buffer B");

        bufferC = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(float) * arraySize, NULL, &error);
        if (error != CL_SUCCESS) throw std::runtime_error("failed to create buffer C");

        // Set kernel arguments
        error  = clSetKernelArg(kernel, 0, sizeof(cl_mem), &bufferA);
        error |= clSetKernelArg(kernel, 1, sizeof(cl_mem), &bufferB);
        error |= clSetKernelArg(kernel, 2, sizeof(cl_mem), &bufferC);
        if (error != CL_SUCCESS) throw std::runtime_error("failed to set kernel arguments");

        // Execute the kernel
        size_t globalWorkSize = arraySize;
        error = clEnqueueNDRangeKernel(queue, kernel, 1, NULL, &globalWorkSize, NULL, 0, NULL, NULL);
        if (error != CL_SUCCESS) throw std::runtime_error("failed to enqueue kernel");

        // Wait for the command queue to complete
        error = clFinish(queue);
        if (error != CL_SUCCESS) throw std::runtime_error("failed to finish command queue");

        // Read back the result
        error = clEnqueueReadBuffer(queue, bufferC, CL_TRUE, 0, sizeof(float) * arraySize, arrayC, 0, NULL, NULL);
        if (error != CL_SUCCESS) throw std::runtime_error("failed to read buffer C");

        // Verify results
        for (int i = 0; i < arraySize; i++) if (arrayC[i] != arrayA[i] + arrayB[i]) {
            throw std::runtime_error("result verification failed at index " + std::to_string(i));
        }
        std::cout << "Vector addition successful and verified!" << std::endl;

        // Clean up OpenCL resources
        if (bufferA) clReleaseMemObject(bufferA);
        if (bufferB) clReleaseMemObject(bufferB);
        if (bufferC) clReleaseMemObject(bufferC);
        if (kernel) clReleaseKernel(kernel);
        if (program) clReleaseProgram(program);
        if (queue) clReleaseCommandQueue(queue);
        if (context) clReleaseContext(context);

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    } catch (...) {
        std::cerr << "An unknown error occurred." << std::endl;
    }

    // Free host arrays
    delete[] arrayA;
    delete[] arrayB;
    delete[] arrayC;
    return 0;
}

