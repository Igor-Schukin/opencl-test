#include <iostream>
#include <CL/cl.h>

// Initialize OpenCL and get the first available device ID
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
        }  catch (const std::exception& e) {
            if (devices) delete[] devices;
            throw;
        }

        delete[] devices;
    } catch (const std::exception& e) {
        if (platforms) delete[] platforms;
        throw;
    }

    delete[] platforms;
    return device;
}

int main() {
    try {
        cl_device_id device = clInit();
        std::cout << "Successfully initialized OpenCL and obtained device ID: " << device << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "OpenCL initialization failed: " << e.what() << std::endl;
        return -1;
    }
    return 0;
}

