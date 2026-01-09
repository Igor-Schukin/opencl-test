#include <iostream>
#include <CL/cl.h>

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
            char platformName[128]{};
            error = clGetPlatformInfo(platforms[i], CL_PLATFORM_NAME, sizeof(platformName), platformName, NULL);
            if (error != CL_SUCCESS) std::cout << "Error getting platform name: " << error << std::endl;
            else std::cout << "Platform Name: " << platformName << std::endl;

            char platformProfile[128]{};
            error = clGetPlatformInfo(platforms[i], CL_PLATFORM_PROFILE, sizeof(platformProfile), platformProfile, NULL);
            if (error != CL_SUCCESS) std::cout << "Error getting platform profile: " << error << std::endl;
            else std::cout << "Platform Profile: " << platformProfile << std::endl;

            char platformVendor[128]{};
            error = clGetPlatformInfo(platforms[i], CL_PLATFORM_VENDOR, sizeof(platformVendor), platformVendor, NULL);
            if (error != CL_SUCCESS) std::cout << "Error getting platform vendor: " << error << std::endl;
            else std::cout << "Platform Vendor: " << platformVendor << std::endl;

            char platformVersion[128]{};
            error = clGetPlatformInfo(platforms[i], CL_PLATFORM_VERSION, sizeof(platformVersion), platformVersion, NULL);
            if (error != CL_SUCCESS) std::cout << "Error getting platform version: " << error << std::endl;
            else std::cout << "Platform Version: " << platformVersion << std::endl;

            char platformExtensions[1024]{};
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

                    char deviceDriverVersion[128]{};
                    error = clGetDeviceInfo(devices[j], CL_DRIVER_VERSION, sizeof(deviceDriverVersion), deviceDriverVersion, NULL);
                    if (error != CL_SUCCESS) std::cout << "Error getting device driver version: " << error << std::endl;
                    else std::cout << "deviceDriverVersion: " << deviceDriverVersion << std::endl;

                    char deviceProfile[128]{};
                    error = clGetDeviceInfo(devices[j], CL_DEVICE_PROFILE, sizeof(deviceProfile), deviceProfile, NULL);
                    if (error != CL_SUCCESS) std::cout << "Error getting device profile: " << error << std::endl;
                    else std::cout << "deviceProfile: " << deviceProfile << std::endl;

                    char deviceVendor[128]{};
                    error = clGetDeviceInfo(devices[j], CL_DEVICE_VENDOR, sizeof(deviceVendor), deviceVendor, NULL);
                    if (error != CL_SUCCESS) std::cout << "Error getting device vendor: " << error << std::endl;
                    else  std::cout << "deviceVendor: " << deviceVendor << std::endl;

                    char deviceVersion[128]{};
                    error = clGetDeviceInfo(devices[j], CL_DEVICE_VERSION, sizeof(deviceVersion), deviceVersion, NULL);
                    if (error != CL_SUCCESS) std::cout << "Error getting device version: " << error << std::endl;
                    else std::cout << "deviceVersion: " << deviceVersion << std::endl;

                    char deviceName[128]{};
                    error = clGetDeviceInfo(devices[j], CL_DEVICE_NAME, sizeof(deviceName), deviceName, NULL);
                    if (error != CL_SUCCESS) std::cout << "Error getting device name: " << error << std::endl;
                    else std::cout << "deviceName: " << deviceName << std::endl;

                    cl_uint deviceAddressBits;
                    error = clGetDeviceInfo(devices[j], CL_DEVICE_ADDRESS_BITS, sizeof(deviceAddressBits), &deviceAddressBits, NULL);
                    if (error != CL_SUCCESS) std::cout << "Error getting device address bits: " << error << std::endl;
                    else std::cout << "deviceAddressBits: " << deviceAddressBits << std::endl;

                    cl_bool deviceAvailable;
                    error = clGetDeviceInfo(devices[j], CL_DEVICE_AVAILABLE, sizeof(deviceAvailable), &deviceAvailable, NULL);
                    if (error != CL_SUCCESS) std::cout << "Error getting device available: " << error << std::endl;
                    else std::cout << "deviceAvailable: " << (deviceAvailable ? "True" : "False") << std::endl;

                    cl_bool deviceCompilerAvailable;
                    error = clGetDeviceInfo(devices[j], CL_DEVICE_COMPILER_AVAILABLE, sizeof(deviceCompilerAvailable), &deviceCompilerAvailable, NULL);
                    if (error != CL_SUCCESS) std::cout << "Error getting device compiler available: " << error << std::endl;
                    else std::cout << "deviceCompilerAvailable: " << (deviceCompilerAvailable ? "True" : "False") << std::endl;

                    cl_uint deviceGlobalMemCachelineSize;
                    error = clGetDeviceInfo(devices[j], CL_DEVICE_GLOBAL_MEM_CACHELINE_SIZE, sizeof(deviceGlobalMemCachelineSize), &deviceGlobalMemCachelineSize, NULL);
                    if (error != CL_SUCCESS) std::cout << "Error getting device global mem cacheline size: " << error << std::endl;
                    else std::cout << "deviceGlobalMemCachelineSize: " << deviceGlobalMemCachelineSize << std::endl;

                    cl_platform_id devicePlatform;
                    error = clGetDeviceInfo(devices[j], CL_DEVICE_PLATFORM, sizeof(devicePlatform), &devicePlatform, NULL);
                    if (error != CL_SUCCESS) std::cout << "Error getting device platform: " << error << std::endl;
                    else std::cout << "devicePlatform: " << devicePlatform << std::endl;

                    cl_device_type deviceType;
                    error = clGetDeviceInfo(devices[j], CL_DEVICE_TYPE, sizeof(deviceType), &deviceType, NULL);
                    if (error != CL_SUCCESS) std::cout << "Error getting device type: " << error << std::endl;
                    else std::cout << "deviceType: " << deviceType << std::endl;

                    cl_uint deviceVendorId;
                    error = clGetDeviceInfo(devices[j], CL_DEVICE_VENDOR_ID, sizeof(deviceVendorId), &deviceVendorId, NULL);
                    if (error != CL_SUCCESS) std::cout << "Error getting device vendor id: " << error << std::endl;
                    else std::cout << "deviceVendorId: " << deviceVendorId << std::endl;

                    cl_uint deviceMaxComputeUnits;
                    error = clGetDeviceInfo(devices[j], CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(deviceMaxComputeUnits), &deviceMaxComputeUnits, NULL);
                    if (error != CL_SUCCESS) std::cout << "Error getting device max compute units: " << error << std::endl;
                    else std::cout << "deviceMaxComputeUnits: " << deviceMaxComputeUnits << std::endl;

                    size_t deviceMaxWorkGroupSize;
                    error = clGetDeviceInfo(devices[j], CL_DEVICE_MAX_WORK_GROUP_SIZE, sizeof(deviceMaxWorkGroupSize), &deviceMaxWorkGroupSize, NULL);
                    if (error != CL_SUCCESS) std::cout << "Error getting device max work group size: " << error << std::endl;
                    else std::cout << "deviceMaxWorkGroupSize: " << deviceMaxWorkGroupSize << std::endl;

                    std::cout << "------------------------" << std::endl;
                    char deviceExtensions[2048]{};
                    error = clGetDeviceInfo(devices[j], CL_DEVICE_EXTENSIONS, sizeof(deviceExtensions), deviceExtensions, NULL);
                    if (error != CL_SUCCESS) std::cout << "Error getting device extensions: " << error << std::endl;
                    else std::cout << "deviceExtensions: " << deviceExtensions << std::endl;

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