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
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }

    if (platforms) delete[] platforms;
    return 0;
}