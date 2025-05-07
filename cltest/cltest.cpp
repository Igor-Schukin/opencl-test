#include <iostream>
#include <CL/cl.h>

int main() {
    cl_int err;
    cl_uint num_platforms;

    err = clGetPlatformIDs(0, NULL, &num_platforms);
    if (err != CL_SUCCESS || num_platforms == 0) {
        std::cout << "OpenCL not found in the system or no platforms available" << std::endl;
        return 1;
    }

    std::cout << "OpenCL platforms found: " << num_platforms << std::endl;

    cl_platform_id* platforms = new cl_platform_id[num_platforms];
    err = clGetPlatformIDs(num_platforms, platforms, NULL);
    if (err != CL_SUCCESS) {
        std::cout << "Error retrieving platform information" << std::endl;
        delete[] platforms;
        return 1;
    }

    for (cl_uint i = 0; i < num_platforms; i++) {
        char platform_name[128];
        char platform_vendor[128];
        
        clGetPlatformInfo(platforms[i], CL_PLATFORM_NAME, sizeof(platform_name), platform_name, NULL);
        clGetPlatformInfo(platforms[i], CL_PLATFORM_VENDOR, sizeof(platform_vendor), platform_vendor, NULL);

        std::cout << "\nPlatform " << i + 1 << ":" << std::endl;
        std::cout << "Name: " << platform_name << std::endl;
        std::cout << "Manufacturer: " << platform_vendor << std::endl;

        cl_uint num_devices;
        err = clGetDeviceIDs(platforms[i], CL_DEVICE_TYPE_ALL, 0, NULL, &num_devices);
        if (err != CL_SUCCESS || num_devices == 0) {
            std::cout << "No devices found for this platform" << std::endl;
            continue;
        }

        std::cout << "Devices found: " << num_devices << std::endl;

        cl_device_id* devices = new cl_device_id[num_devices];
        err = clGetDeviceIDs(platforms[i], CL_DEVICE_TYPE_ALL, num_devices, devices, NULL);
        if (err != CL_SUCCESS) {
            std::cout << "Error while getting devices" << std::endl;
            delete[] devices;
            continue;
        }

        for (cl_uint j = 0; j < num_devices; j++) {
            char device_name[128];
            cl_device_type device_type;
            
            clGetDeviceInfo(devices[j], CL_DEVICE_NAME, sizeof(device_name), device_name, NULL);
            clGetDeviceInfo(devices[j], CL_DEVICE_TYPE, sizeof(device_type), &device_type, NULL);

            std::cout << "  Device " << j + 1 << ":" << std::endl;
            std::cout << "    Name: " << device_name << std::endl;
            std::cout << "    Type: ";
            if (device_type & CL_DEVICE_TYPE_CPU) std::cout << "CPU ";
            if (device_type & CL_DEVICE_TYPE_GPU) std::cout << "GPU ";
            if (device_type & CL_DEVICE_TYPE_ACCELERATOR) std::cout << "Accelerator ";
            std::cout << std::endl;
        }

        delete[] devices;
    }

    delete[] platforms;
    std::cout << "ByeÑ" << std::endl;
    return 0;
}