/**
 * @file 2.devices.cpp
 * @author Igor Schukin
 * @date 2026-01-13
 * @brief Example of querying OpenCL platforms and devices in C++
 * @section requirements
 * - OpenCL SDK installed
 * - OpenCL-capable device (GPU/CPU)
 * - C++11 compiler
 * @section usage on HPC server
 * 1. ssh to HPC server: 
 *      ssh <user-name>@ui-2.hpc.rtu.lv
 * 2. load or create file with this code
 *      nano 2.devices.cpp
 * 3. load module with gcc and OpenCL support: 
 *      module load compilers/gcc/gcc-11.2.0-nvptx
 * 4. compile code: 
 *      g++ -std=c++11 -o 2.devices 2.devices.cpp -I/usr/include/CL -L/usr/lib -lOpenCL
 * 5. request interactive session with GPU:
 *      qsub -l nodes=1:ppn=1:gpus=1 -I
 * 6. run the program: 
 *      ./2.devices
 */

#include <iostream>
#include <string>
#include <stdexcept>
#include <CL/cl.h>

/**
 * @brief Buffer sizes for querying OpenCL platform and device information
 */
const size_t SMALL_BUFFER_SIZE = 128;    // For short strings like names, versions
const size_t MEDIUM_BUFFER_SIZE = 1024;  // For platform extensions list
const size_t LARGE_BUFFER_SIZE = 2048;   // For device extensions list

/**
 * @brief Main function to query and display OpenCL platform and device information
 * 
 * @return int 
 */

int main() {
    cl_int error;
    cl_uint numPlatforms;
    cl_platform_id* platforms = nullptr;

    try {
        // Get platform count
        error = clGetPlatformIDs(0, NULL, &numPlatforms);
        std::cout << "Platforms Count: " << numPlatforms << std::endl;
        if (error != CL_SUCCESS || numPlatforms == 0) throw std::runtime_error("No platforms found or error occurred");

        // Get all platforms
        platforms = new cl_platform_id[numPlatforms];
        error = clGetPlatformIDs(numPlatforms, platforms, NULL);
        if (error != CL_SUCCESS) throw std::runtime_error("Failed to get platform IDs");
        std::cout << "====================================" << std::endl;

        // Iterate over each platform and get info
        for (cl_uint i = 0; i < numPlatforms; i++) {
            char platformName[SMALL_BUFFER_SIZE]{};
            error = clGetPlatformInfo(platforms[i], CL_PLATFORM_NAME, sizeof(platformName), platformName, NULL);
            if (error != CL_SUCCESS) std::cout << "Error getting platform name: " << error << std::endl;
            else std::cout << "Platform Name: " << platformName << std::endl;

            char platformProfile[SMALL_BUFFER_SIZE]{};
            error = clGetPlatformInfo(platforms[i], CL_PLATFORM_PROFILE, sizeof(platformProfile), platformProfile, NULL);
            if (error != CL_SUCCESS) std::cout << "Error getting platform profile: " << error << std::endl;
            else std::cout << "Platform Profile: " << platformProfile << std::endl;

            char platformVendor[SMALL_BUFFER_SIZE]{};
            error = clGetPlatformInfo(platforms[i], CL_PLATFORM_VENDOR, sizeof(platformVendor), platformVendor, NULL);
            if (error != CL_SUCCESS) std::cout << "Error getting platform vendor: " << error << std::endl;
            else std::cout << "Platform Vendor: " << platformVendor << std::endl;

            char platformVersion[SMALL_BUFFER_SIZE]{};
            error = clGetPlatformInfo(platforms[i], CL_PLATFORM_VERSION, sizeof(platformVersion), platformVersion, NULL);
            if (error != CL_SUCCESS) std::cout << "Error getting platform version: " << error << std::endl;
            else std::cout << "Platform Version: " << platformVersion << std::endl;

            char platformExtensions[MEDIUM_BUFFER_SIZE]{};
            error = clGetPlatformInfo(platforms[i], CL_PLATFORM_EXTENSIONS, sizeof(platformExtensions), platformExtensions, NULL);
            if (error != CL_SUCCESS) std::cout << "Error getting platform extensions: " << error << std::endl;
            else std::cout << "Platform Extensions: " << platformExtensions << std::endl;

            // Get devices for each platform
            cl_uint numDevices;
            cl_device_id* devices = nullptr;
            try {
                error = clGetDeviceIDs(platforms[i], CL_DEVICE_TYPE_ALL, 0, NULL, &numDevices);
                std::cout << "Devices Count: " << numDevices << std::endl;
                if (error != CL_SUCCESS || numDevices == 0) throw std::runtime_error("Failed to get device count or no devices found");

                devices = new cl_device_id[numDevices];
                error = clGetDeviceIDs(platforms[i], CL_DEVICE_TYPE_ALL, numDevices, devices, NULL);
                if (error != CL_SUCCESS) throw std::runtime_error("Failed to get device IDs");

                // Iterate over each device and get info
                for (cl_uint j = 0; j < numDevices; j++) {
                    std::cout << "------------------------------------" << std::endl;

                    char driverVersion[SMALL_BUFFER_SIZE]{};
                    error = clGetDeviceInfo(devices[j], CL_DRIVER_VERSION, sizeof(driverVersion), driverVersion, NULL);
                    if (error != CL_SUCCESS) std::cout << "Error getting device driver version: " << error << std::endl;
                    else std::cout << "Driver Version: " << driverVersion << std::endl;

                    char deviceProfile[SMALL_BUFFER_SIZE]{};
                    error = clGetDeviceInfo(devices[j], CL_DEVICE_PROFILE, sizeof(deviceProfile), deviceProfile, NULL);
                    if (error != CL_SUCCESS) std::cout << "Error getting device profile: " << error << std::endl;
                    else std::cout << "Device Profile: " << deviceProfile << std::endl;

                    char deviceVendor[SMALL_BUFFER_SIZE]{};
                    error = clGetDeviceInfo(devices[j], CL_DEVICE_VENDOR, sizeof(deviceVendor), deviceVendor, NULL);
                    if (error != CL_SUCCESS) std::cout << "Error getting device vendor: " << error << std::endl;
                    else  std::cout << "Device Vendor: " << deviceVendor << std::endl;

                    char deviceVersion[SMALL_BUFFER_SIZE]{};
                    error = clGetDeviceInfo(devices[j], CL_DEVICE_VERSION, sizeof(deviceVersion), deviceVersion, NULL);
                    if (error != CL_SUCCESS) std::cout << "Error getting device version: " << error << std::endl;
                    else std::cout << "Device Version: " << deviceVersion << std::endl;

                    char deviceName[SMALL_BUFFER_SIZE]{};
                    error = clGetDeviceInfo(devices[j], CL_DEVICE_NAME, sizeof(deviceName), deviceName, NULL);
                    if (error != CL_SUCCESS) std::cout << "Error getting device name: " << error << std::endl;
                    else std::cout << "Device Name: " << deviceName << std::endl;

                    cl_uint addressBits;
                    error = clGetDeviceInfo(devices[j], CL_DEVICE_ADDRESS_BITS, sizeof(addressBits), &addressBits, NULL);
                    if (error != CL_SUCCESS) std::cout << "Error getting device address bits: " << error << std::endl;
                    else std::cout << "Address Bits: " << addressBits << std::endl;

                    cl_bool isAvailable;
                    error = clGetDeviceInfo(devices[j], CL_DEVICE_AVAILABLE, sizeof(isAvailable), &isAvailable, NULL);
                    if (error != CL_SUCCESS) std::cout << "Error getting device available: " << error << std::endl;
                    else std::cout << "Device Available: " << (isAvailable ? "True" : "False") << std::endl;

                    cl_bool compilerAvailable;
                    error = clGetDeviceInfo(devices[j], CL_DEVICE_COMPILER_AVAILABLE, sizeof(compilerAvailable), &compilerAvailable, NULL);
                    if (error != CL_SUCCESS) std::cout << "Error getting device compiler available: " << error << std::endl;
                    else std::cout << "Compiler Available: " << (compilerAvailable ? "True" : "False") << std::endl;

                    cl_uint globalMemCachelineSize;
                    error = clGetDeviceInfo(devices[j], CL_DEVICE_GLOBAL_MEM_CACHELINE_SIZE, sizeof(globalMemCachelineSize), &globalMemCachelineSize, NULL);
                    if (error != CL_SUCCESS) std::cout << "Error getting device global mem cacheline size: " << error << std::endl;
                    else std::cout << "Global Mem Cacheline Size: " << globalMemCachelineSize << std::endl;

                    cl_platform_id devicePlatform;
                    error = clGetDeviceInfo(devices[j], CL_DEVICE_PLATFORM, sizeof(devicePlatform), &devicePlatform, NULL);
                    if (error != CL_SUCCESS) std::cout << "Error getting device platform: " << error << std::endl;
                    else std::cout << "Device Platform ID: " << devicePlatform << std::endl;

                    cl_device_type deviceType;
                    error = clGetDeviceInfo(devices[j], CL_DEVICE_TYPE, sizeof(deviceType), &deviceType, NULL);
                    if (error != CL_SUCCESS) std::cout << "Error getting device type: " << error << std::endl;
                    else std::cout << "Device Type: " << deviceType << std::endl;

                    cl_uint vendorId;
                    error = clGetDeviceInfo(devices[j], CL_DEVICE_VENDOR_ID, sizeof(vendorId), &vendorId, NULL);
                    if (error != CL_SUCCESS) std::cout << "Error getting device vendor id: " << error << std::endl;
                    else std::cout << "Vendor ID: " << vendorId << std::endl;

                    cl_uint maxComputeUnits;
                    error = clGetDeviceInfo(devices[j], CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(maxComputeUnits), &maxComputeUnits, NULL);
                    if (error != CL_SUCCESS) std::cout << "Error getting device max compute units: " << error << std::endl;
                    else std::cout << "Max Compute Units: " << maxComputeUnits << std::endl;

                    size_t maxWorkGroupSize;
                    error = clGetDeviceInfo(devices[j], CL_DEVICE_MAX_WORK_GROUP_SIZE, sizeof(maxWorkGroupSize), &maxWorkGroupSize, NULL);
                    if (error != CL_SUCCESS) std::cout << "Error getting device max work group size: " << error << std::endl;
                    else std::cout << "Max Work Group Size: " << maxWorkGroupSize << std::endl;

                    std::cout << "------------------------" << std::endl;
                    char deviceExtensions[LARGE_BUFFER_SIZE]{};
                    error = clGetDeviceInfo(devices[j], CL_DEVICE_EXTENSIONS, sizeof(deviceExtensions), deviceExtensions, NULL);
                    if (error != CL_SUCCESS) std::cout << "Error getting device extensions: " << error << std::endl;
                    else std::cout << "Device Extensions: " << deviceExtensions << std::endl;

                }
            } catch (const std::exception& e) {
                std::cerr << e.what() << std::endl;
            }
            if (devices) delete[] devices;

            std::cout << "====================================" << std::endl;
        }
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
    }

    if (platforms) delete[] platforms;
    return 0;
}