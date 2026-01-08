#include <iostream>
#include <CL/cl.h>

/*
            ErrorCode error = Cl.GetPlatformIDs(0, null, out platformCount);

            richTextBox1.AppendText("Platforms Count: " + platformCount + "\r\n");
            richTextBox1.AppendText("Result/Error 1. Cl.GetPlatformIDs: " + error.ToString() + "\r\n");
            platforms = Cl.GetPlatformIDs(out error);
     
            richTextBox1.AppendText("Result/Error 2. Cl.GetPlatformIDs: " + error.ToString() + "\r\n");
            richTextBox1.AppendText("------------------------------------\r\n");

            // get all platforms
            foreach (OpenCL.Net.Platform platform in platforms)
            {
                string platformName = Cl.GetPlatformInfo(platform, PlatformInfo.Name, out error).ToString();
                string platformProfile = Cl.GetPlatformInfo(platform, PlatformInfo.Profile, out error).ToString();
                string platformVendor = Cl.GetPlatformInfo(platform, PlatformInfo.Vendor, out error).ToString();
                string platformVersion = Cl.GetPlatformInfo(platform, PlatformInfo.Version, out error).ToString();
                string platformExtensions = Cl.GetPlatformInfo(platform, PlatformInfo.Extensions, out error).ToString();

                richTextBox1.AppendText("Result/Error Cl.GetPlatformInfo: " + error.ToString() + "\r\n");
                richTextBox1.AppendText("Platform_Name: " + platformName + "\r\n");
                richTextBox1.AppendText("Platform_Profile: " + platformProfile + "\r\n");
                richTextBox1.AppendText("Platform_Vendor: " + platformVendor + "\r\n");
                richTextBox1.AppendText("Platform_Version: " + platformVersion + "\r\n");
                // richTextBox1.AppendText("Platform_Extensions: " + platformExtensions + "\r\n");
                richTextBox1.AppendText("------------------------------------\r\n");
            }
*/

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
        char platform_name[128];
        char platform_profile[128];
        char platform_vendor[128];
        char platform_version[128];
        char platform_extensions[512];
        
        err = clGetPlatformInfo(platforms[i], CL_PLATFORM_NAME, sizeof(platform_name), platform_name, NULL);
        clGetPlatformInfo(platforms[i], CL_PLATFORM_PROFILE, sizeof(platform_profile), platform_profile, NULL);
        clGetPlatformInfo(platforms[i], CL_PLATFORM_VENDOR, sizeof(platform_vendor), platform_vendor, NULL);
        clGetPlatformInfo(platforms[i], CL_PLATFORM_VERSION, sizeof(platform_version), platform_version, NULL);
        clGetPlatformInfo(platforms[i], CL_PLATFORM_EXTENSIONS, sizeof(platform_extensions), platform_extensions, NULL);

        std::cout << "Result/Error Cl.GetPlatformInfo: " << err << std::endl;
        std::cout << "Platform Name: " << platform_name << std::endl;
        std::cout << "Platform Profile: " << platform_profile << std::endl;
        std::cout << "Platform Vendor: " << platform_vendor << std::endl;
        std::cout << "Platform Version: " << platform_version << std::endl;
        std::cout << "Platform Extensions: " << platform_extensions << std::endl;
        std::cout << "------------------------------------" << std::endl;

        std::cout << "\nPlatform " << i + 1 << ":" << std::endl;
        std::cout << "Name: " << platform_name << std::endl;
        std::cout << "Manufacturer: " << platform_vendor << std::endl;
    }

    delete[] platforms;
    return 0;
}