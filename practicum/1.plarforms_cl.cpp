#include <iostream>
#include <CL/cl.h>

int main() {
    cl_int error;
    cl_uint num_platforms;

    // Get platform count
    error = clGetPlatformIDs(0, NULL, &num_platforms);
    std::cout << "Platforms Count: " << num_platforms << std::endl;
    std::cout << "Result/Error 1. Cl.GetPlatformIDs: " << error << std::endl;

    // Get all platforms
    cl_platform_id* platforms = new cl_platform_id[num_platforms];
    error = clGetPlatformIDs(num_platforms, platforms, NULL);
    std::cout << "Result/Error 2. Cl.GetPlatformIDs: " << error << std::endl;
    std::cout << "------------------------------------" << std::endl;

    // Iterate over each platform and get info
    for (cl_uint i = 0; i < num_platforms; i++) {
        char platform_name[128]{};
        error = clGetPlatformInfo(platforms[i], CL_PLATFORM_NAME, sizeof(platform_name), platform_name, NULL);
        if (error != CL_SUCCESS) std::cout << "Error getting platform name: " << error << std::endl;
        else std::cout << "Platform Name: " << platform_name << std::endl;

        char platform_profile[128]{};
        error = clGetPlatformInfo(platforms[i], CL_PLATFORM_PROFILE, sizeof(platform_profile), platform_profile, NULL);
        if (error != CL_SUCCESS) std::cout << "Error getting platform profile: " << error << std::endl;
        else std::cout << "Platform Profile: " << platform_profile << std::endl;

        char platform_vendor[128]{};
        error = clGetPlatformInfo(platforms[i], CL_PLATFORM_VENDOR, sizeof(platform_vendor), platform_vendor, NULL);
        if (error != CL_SUCCESS) std::cout << "Error getting platform vendor: " << error << std::endl;
        else std::cout << "Platform Vendor: " << platform_vendor << std::endl;

        char platform_version[128]{};
        error = clGetPlatformInfo(platforms[i], CL_PLATFORM_VERSION, sizeof(platform_version), platform_version, NULL);
        if (error != CL_SUCCESS) std::cout << "Error getting platform version: " << error << std::endl;
        else std::cout << "Platform Version: " << platform_version << std::endl;

        char platform_extensions[1024]{};
        error = clGetPlatformInfo(platforms[i], CL_PLATFORM_EXTENSIONS, sizeof(platform_extensions), platform_extensions, NULL);
        if (error != CL_SUCCESS) std::cout << "Error getting platform extensions: " << error << std::endl;
        else std::cout << "Platform Extensions: " << platform_extensions << std::endl;

        std::cout << "------------------------------------" << std::endl;
    }

    delete[] platforms;
    return 0;
}