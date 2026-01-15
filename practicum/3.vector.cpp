/**
 * @file 3.vector.cpp
 * @author Igor Schukin
 * @date 2026-01-13
 * @brief 
 * Example of vector addition using OpenCL in C++
 * This program initializes two arrays with random float values,
 * performs vector addition on an OpenCL device, and verifies the result.
 * It includes error handling for OpenCL operations.
 * @section requirements
 * - OpenCL SDK installed
 * - OpenCL-capable device (GPU/CPU)
 * - C++11 compiler
 * @section usage on HPC server
 * 1. ssh to HPC server: 
 *      ssh <user-name>@ui-2.hpc.rtu.lv
 * 2. load or create file with this code
 *      nano 3.vector.cpp
 * 3. load module with gcc and OpenCL support: 
 *      module load compilers/gcc/gcc-11.2.0-nvptx
 * 4. compile code: 
 *      g++ -std=c++11 -o 3.vector 3.vector.cpp -I/usr/include/CL -L/usr/lib -lOpenCL
 * 5. request interactive session with GPU:
 *      qsub -l nodes=1:ppn=1:gpus=1 -I
 * 6. run the program: 
 *      ./3.vector
 */

#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <CL/cl.h>

/**
 * @brief Check OpenCL and get the first available device ID or throw an error
 * 
 * @return cl_device_id 
 */


cl_device_id getDeviceId() {
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

/**
 * @brief OpenCL kernel source for vector addition
 */

const char* vectorAddKernelSource = R"(
    __kernel void vector_add
    (
        __global const float* a, 
        __global const float* b, 
        __global float* c
    ) 
    {
        int id = get_global_id(0);
        c[id] = a[id] + b[id];
    }
)";

/**
 * @brief Main function to perform vector addition using OpenCL
 * 
 * @return int 
 */

int main() {

    // Host arrays
    const int arraySize = 1024; // Size of the arrays >= 10
    const float minRandomValue = 0.0f;
    const float maxRandomValue = 10.0f;
    float* arrayA = new float[arraySize]; // Input array A
    float* arrayB = new float[arraySize]; // Input array B
    float* arrayC = new float[arraySize]; // Output array C

    // Initialize input arrays with random values in range [0, 10]
    srand(static_cast<unsigned int>(time(0)));
    for (int i = 0; i < arraySize; i++) {
        arrayA[i] = minRandomValue + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX/(maxRandomValue - minRandomValue)));
        arrayB[i] = minRandomValue + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX/(maxRandomValue - minRandomValue)));
    }

    // OpenCL resources
    cl_context context = nullptr;
    cl_command_queue queue = nullptr;
    cl_program program = nullptr;
    cl_kernel kernel = nullptr;
    cl_mem bufferA = nullptr;
    cl_mem bufferB = nullptr;
    cl_mem bufferC = nullptr;

    try {
        cl_device_id device = getDeviceId();
        cl_int error;

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

        //Show first 10 results
        std::cout << "First 10 results of vector addition:" << std::endl;
        for (int i = 0; i < 10; i++) {
            std::cout << arrayA[i] << " + " << arrayB[i] << " = " << arrayC[i] << std::endl;
        }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    } catch (...) {
        std::cerr << "An unknown error occurred." << std::endl;
    }

    // Clean up OpenCL resourcess
    if (bufferA) clReleaseMemObject(bufferA);
    if (bufferB) clReleaseMemObject(bufferB);
    if (bufferC) clReleaseMemObject(bufferC);
    if (kernel) clReleaseKernel(kernel);
    if (program) clReleaseProgram(program);
    if (queue) clReleaseCommandQueue(queue);
    if (context) clReleaseContext(context);

    // Free host arrays
    delete[] arrayA;
    delete[] arrayB;
    delete[] arrayC;
    return 0;
}