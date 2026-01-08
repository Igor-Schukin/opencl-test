#include <iostream>
#include <CL/cl.h>

int main() {
    cl_int err;
    cl_uint num_platforms;

    // Get platform count
    err = clGetPlatformIDs(0, NULL, &num_platforms);
    std::cout << "Platforms Count: " << num_platforms << std::endl;
    std::cout << "Result/Error 1. Cl.GetPlatformIDs: " << err << std::endl;

    // Get all platforms
    cl_platform_id* platforms = new cl_platform_id[num_platforms];
    err = clGetPlatformIDs(num_platforms, platforms, NULL);
    std::cout << "Result/Error 2. Cl.GetPlatformIDs: " << err << std::endl;
    std::cout << "------------------------------------" << std::endl;

    // Iterate over each platform and get info
    for (cl_uint i = 0; i < num_platforms; i++) {
        char platform_name[128]{};
        char platform_profile[128]{};
        char platform_vendor[128]{};
        char platform_version[128]{};
        char platform_extensions[1024]{};
        
        err = clGetPlatformInfo(platforms[i], CL_PLATFORM_NAME, sizeof(platform_name), platform_name, NULL);
        clGetPlatformInfo(platforms[i], CL_PLATFORM_PROFILE, sizeof(platform_profile), platform_profile, NULL);
        clGetPlatformInfo(platforms[i], CL_PLATFORM_VENDOR, sizeof(platform_vendor), platform_vendor, NULL);
        clGetPlatformInfo(platforms[i], CL_PLATFORM_VERSION, sizeof(platform_version), platform_version, NULL);
        clGetPlatformInfo(platforms[i], CL_PLATFORM_EXTENSIONS, sizeof(platform_extensions), platform_extensions, NULL);

        std::cout << "Platform Name: " << platform_name << std::endl;
        std::cout << "Platform Profile: " << platform_profile << std::endl;
        std::cout << "Platform Vendor: " << platform_vendor << std::endl;
        std::cout << "Platform Version: " << platform_version << std::endl;
        std::cout << "Platform Extensions: " << platform_extensions << std::endl;
        std::cout << "------------------------------------" << std::endl;
    }

    delete[] platforms;
    return 0;
}