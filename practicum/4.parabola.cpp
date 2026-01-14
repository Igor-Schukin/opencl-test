/**
 * @file 4.parabola.cpp
 * @author Igor Schukin
 * @date 2026-01-14
 * @brief 
 * Example of filling an array with values of a quadratic parabola using OpenCL in C++
 * This program computes the values of a quadratic parabola y(x) in the range x=[-1, 1] at N points,
 * where y(x) = ax^2 + bx + c, with user-entered coefficients a, b, c and number of points N.
 * It performs the parallel computation on an OpenCL device and verifies the result.
 * The program includes error handling for OpenCL operations.
 * @section requirements
 * - OpenCL SDK installed
 * - OpenCL-capable device (GPU/CPU)
 * - C++11 compiler
 * @section usage on HPC server
 * 1. ssh to HPC server: 
 *      ssh <user-name>@ui-2.hpc.rtu.lv
 * 2. load or create file with this code
 *      nano 4.parabola.cpp
 * 3. load module with gcc and OpenCL support: 
 *      module load compilers/gcc/gcc-11.2.0-nvptx
 * 4. compile code: 
 *      g++ -std=c++11 -o 4.parabola 4.parabola.cpp -I/usr/include/CL -L/usr/lib -lOpenCL
 * 5. request interactive session with GPU:
 *      qsub -l nodes=1:ppn=1:gpus=1 -I
 * 6. run the program: 
 *      ./4.parabola
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
 * @brief OpenCL kernel source for computing values of a quadratic parabola
 */

const char* parabolaKernelSource = R"(
    __kernel void compute_parabola
    (
        const float a, 
        const float b, 
        const float c,
        const float xmin,
        const float xmax,
        const int N,
        __global float* y
    ) 
    {
        int id = get_global_id(0);
        float x = xmin + id * (xmax - xmin) / (N - 1);
        y[id] = a * x * x + b * x + c;
    }
)";

/**
 * @brief Main function to perform parabola calculation using OpenCL
 * 
 * @return int 
 */

int main() {

    // Hardcoded values
    const float minX = -1.0f;
    const float maxX = 1.0f;

    // User-defned values
    float a, b, c;
    int pointsCount;
    std::cout << "a = ";  std::cin >> a;
    std::cout << "b = ";  std::cin >> b;
    std::cout << "c = ";  std::cin >> c;
    std::cout << "Number of points = ";  std::cin >> pointsCount;

    // Allocate result array
    float* y = new float[pointsCount];

    // OpenCL resources
    cl_context context = nullptr;
    cl_command_queue queue = nullptr;
    cl_program program = nullptr;
    cl_kernel kernel = nullptr;
    cl_mem bufferY = nullptr;

    try {
        cl_device_id device = getDeviceId();
        cl_int error;

        // Prepare OpenCL context, command queue, program and kernel
        context = clCreateContext(NULL, 1, &device, NULL, NULL, &error);
        if (error != CL_SUCCESS) throw std::runtime_error("failed to create OpenCL context");

        queue = clCreateCommandQueueWithProperties(context, device, NULL, &error);
        if (error != CL_SUCCESS) throw std::runtime_error("failed to create command queue");

        program = clCreateProgramWithSource(context, 1, &parabolaKernelSource, NULL, &error);
        if (error != CL_SUCCESS) throw std::runtime_error("failed to create program");

        error = clBuildProgram(program, 1, &device, NULL, NULL, NULL);
        if (error != CL_SUCCESS) throw std::runtime_error("failed to build program");

        kernel = clCreateKernel(program, "compute_parabola", &error);
        if (error != CL_SUCCESS) throw std::runtime_error("failed to create kernel");

        // Create output buffer
        bufferY = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(float) * pointsCount, NULL, &error);
        if (error != CL_SUCCESS) throw std::runtime_error("failed to create buffer Y");

        // Set kernel arguments
        error  = clSetKernelArg(kernel, 0, sizeof(float), &a)
               | clSetKernelArg(kernel, 1, sizeof(float), &b)
               | clSetKernelArg(kernel, 2, sizeof(float), &c)
               | clSetKernelArg(kernel, 3, sizeof(float), &minX)
               | clSetKernelArg(kernel, 4, sizeof(float), &maxX)
               | clSetKernelArg(kernel, 5, sizeof(int), &pointsCount)
               | clSetKernelArg(kernel, 6, sizeof(cl_mem), &bufferY);
        if (error != CL_SUCCESS) throw std::runtime_error("failed to set kernel arguments");

        // Execute the kernel
        size_t globalWorkSize = pointsCount;
        error = clEnqueueNDRangeKernel(queue, kernel, 1, NULL, &globalWorkSize, NULL, 0, NULL, NULL);
        if (error != CL_SUCCESS) throw std::runtime_error("failed to enqueue kernel");

        // Wait for the command queue to complete
        error = clFinish(queue);
        if (error != CL_SUCCESS) throw std::runtime_error("failed to finish command queue");

        // Read back the result
        error = clEnqueueReadBuffer(queue, bufferY, CL_TRUE, 0, sizeof(float) * pointsCount, (void*)y, 0, NULL, NULL);
        if (error != CL_SUCCESS) throw std::runtime_error("failed to read buffer Y");

        // Verify results
        for (int i = 0; i < pointsCount; i++) {
            float x = minX + i * (maxX - minX) / (pointsCount - 1);
            float expected = a * x * x + b * x + c;
            if (std::abs(y[i] - expected) > 1e-5)
                throw std::runtime_error("result verification failed at index " + std::to_string(i));
        }
        std::cout << "Parabola calculation successful and verified!" << std::endl;

        //Show first 10 results
        std::cout << "First 10 results of parabola calculation:" << std::endl;
        for (int i = 0; i < std::min(10, pointsCount); i++) {
            std::cout << minX + i * (maxX - minX) / (pointsCount - 1) << " -> " << y[i] << std::endl;
        }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    } catch (...) {
        std::cerr << "An unknown error occurred." << std::endl;
    }

    // Clean up OpenCL resources
    if (bufferY) clReleaseMemObject(bufferY);
    if (kernel) clReleaseKernel(kernel);
    if (program) clReleaseProgram(program);
    if (queue) clReleaseCommandQueue(queue);
    if (context) clReleaseContext(context);

    // Free host array
    delete[] y;
    return 0;
}