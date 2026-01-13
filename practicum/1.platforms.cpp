/**
 * @file 1.platforms.cpp
 * @author Igor Schukin
 * @date 2026-01-13
 * @brief Example of querying OpenCL platforms in C++
 * @section requirements
 * - OpenCL SDK installed
 * - OpenCL-capable device (GPU/CPU)
 * - C++11 compiler
 * @section usage on HPC server
 * 1. ssh to HPC server: 
 *      ssh <user-name>@ui-2.hpc.rtu.lv
 * 2. load or create file with this code
 *      nano 1.platforms.cpp
 * 3. load module with gcc and OpenCL support: 
 *      module load compilers/gcc/gcc-11.2.0-nvptx
 * 4. compile code: 
 *      g++ -std=c++11 -o 1.platforms 1.platforms.cpp -I/usr/include/CL -L/usr/lib -lOpenCL
 * 5. request interactive session with GPU:
 *      qsub -l nodes=1:ppn=1:gpus=1 -I
 * 6. run the program: 
 *      ./1.platforms
 */

#include <iostream>
#include <string>
#include <stdexcept>
#include <CL/cl.h>

/**
 * @brief Buffer sizes for querying OpenCL platform information
 */
const size_t SMALL_BUFFER_SIZE = 128;   // For short strings like names, versions
const size_t LARGE_BUFFER_SIZE = 1024;  // For platform extensions list

/**
 * @brief Main function to query and display OpenCL platform information
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
        std::cout << "------------------------------------" << std::endl;

        // Iterate over each platform and get info
        for (cl_uint i = 0; i < numPlatforms; i++) {
            std::cout << "Platform #" << i << std::endl;
            
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

            char platformExtensions[LARGE_BUFFER_SIZE]{};
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