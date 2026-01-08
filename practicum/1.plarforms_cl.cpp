#include <iostream>
#include <CL/cl.h>

int main() {
    cl_int error;
    cl_uint numPlatforms;

    // Get platform count
    error = clGetPlatformIDs(0, NULL, &numPlatforms);
    std::cout << "Platforms Count: " << numPlatforms << std::endl;
    std::cout << "Result/Error 1. Cl.GetPlatformIDs: " << error << std::endl;

    // Get all platforms
    cl_platform_id* platforms = new cl_platform_id[numPlatforms];
    error = clGetPlatformIDs(numPlatforms, platforms, NULL);
    std::cout << "Result/Error 2. Cl.GetPlatformIDs: " << error << std::endl;
    std::cout << "------------------------------------" << std::endl;

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

        std::cout << "------------------------------------" << std::endl;
    }

    delete[] platforms;
    return 0;
}