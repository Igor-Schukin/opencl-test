#include <stdio.h>
#include <stdlib.h>
#include "opencl.h"
#include <sys/time.h>

//~~~~~ Get current time in milliseconds ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

size_t getTime()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (size_t)tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

//~~~~~ OpenCL error class ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

OpenClError::OpenClError(cl_int err, const std::string& operation)
    : std::runtime_error("OpenCL error during " + operation + ": " + std::to_string(err)) {}

OpenClError::OpenClError(const std::string& message)
    : std::runtime_error(message) {}

//~~~~~ Constructors and destructor ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

OpenCL::OpenCL(const std::string & kernelSourceFile, const std::string & kernelName) 
{
    try {
        init(kernelSourceFile, { kernelName });
    }
    catch (...) {
        release();
        throw;
    }
}

OpenCL::OpenCL(const std::string& kernelSourceFile, const std::vector<std::string>& kernelNames)
{
    try {
        init(kernelSourceFile, kernelNames);
    }
    catch (...) {
        release();
        throw;
    }
}


OpenCL::~OpenCL() {
    release();
}

//~~~~~ Load kernel source from file ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

char* OpenCL::loadKernelSource(const std::string& filename)
{
    FILE *file = nullptr;
    char *kernelSource = nullptr;
    try {
        file = fopen(filename.c_str(), "r");
        if (!file) throw OpenClError("Failed to load kernel file");
        fseek(file, 0, SEEK_END);
        size_t file_size = ftell(file);
        rewind(file);
    
        kernelSource = (char*)malloc(file_size + 1);
        if (!kernelSource) throw OpenClError("Failed to allocate memory for kernel source");
        fread(kernelSource, 1, file_size, file);
        kernelSource[file_size] = '\0';
        fclose(file);
    }
    catch (...) {
        if (file) fclose(file);
        if (kernelSource) free(kernelSource);
        throw;
    }
    return kernelSource;
}

//~~~~~ Check OpenCL error and throw exception ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void OpenCL::checkError(cl_int err, const std::string& operation)
{
    if (err != CL_SUCCESS) throw OpenClError(err, operation);
}

//~~~~~ Initialize OpenCL context, command queue, program, and kernel ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void OpenCL::init(const std::string & kernelSourceFile, const std::vector<std::string>& kernelNames) 
{
    // Get platform
    cl_int err = clGetPlatformIDs(1, &_platform, NULL);
    checkError(err, "clGetPlatformIDs");

    // Get device
    err = clGetDeviceIDs(_platform, CL_DEVICE_TYPE_GPU, 1, &_device, NULL);
    checkError(err, "clGetDeviceIDs");

    // Create context
    _context = clCreateContext(NULL, 1, &_device, NULL, NULL, &err);
    checkError(err, "clCreateContext");

    // Create command queue
    _queue = clCreateCommandQueueWithProperties(_context, _device, NULL, &err);
    checkError(err, "clCreateCommandQueueWithProperties");

    // Create program
    char* kernelSource = loadKernelSource(kernelSourceFile);
    _program = clCreateProgramWithSource(_context, 1, (const char**)&kernelSource, NULL, &err);
    free(kernelSource);
    checkError(err, "clCreateProgramWithSource");

    // Build program
    err = clBuildProgram(_program, 1, &_device, NULL, NULL, NULL);
    if (err != CL_SUCCESS)
    {
        size_t log_size;
        clGetProgramBuildInfo(_program, _device, CL_PROGRAM_BUILD_LOG, 0, NULL, &log_size);
        char *log = (char *)malloc(log_size);
        clGetProgramBuildInfo(_program, _device, CL_PROGRAM_BUILD_LOG, log_size, log, NULL);
        std::string errorMsg = "Build error" + std::string(log);
        free(log);
        throw OpenClError(errorMsg);
    }

    // Create kernels

    for (const auto& kernelName : kernelNames)
    {
        cl_kernel kernel = clCreateKernel(_program, kernelName.c_str(), &err);
        checkError(err, "clCreateKernel");
        _kernels.push_back(kernel);
    }
}

//~~~~~ Release OpenCL resources ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void OpenCL::release() 
{
    for (const auto &kernel : _kernels) clReleaseKernel(kernel);
    _kernels.clear();
    if (_program)  clReleaseProgram(_program);
    if (_queue)   clReleaseCommandQueue(_queue);
    if (_context) clReleaseContext(_context);
    freeBuffers();
}

/**************************************************************************************************
 * OpenCL buffer management
 *
 * This section contains functions to create, read, and free OpenCL buffers.
 * The buffers are created based on the argument types specified in the run method.
 * The buffers are automatically freed after use.
 *
 **************************************************************************************************/

//~~~~~ Create buffers for kernel arguments ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void OpenCL::createBuffers(std::vector<std::tuple<ArgTypes, void*, size_t>> args) 
{
    try {
        cl_int err;
        freeBuffers();
        for (const auto& arg : args)
        {
            cl_mem buffer;
            ArgTypes type = std::get<0>(arg);
            void* value = std::get<1>(arg);
            size_t size = std::get<2>(arg);

            switch (type)
            {
                case ArgTypes::IN_IBUF:
                    buffer = clCreateBuffer(_context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(int) * size, value, &err);
                    break;
                case ArgTypes::OUT_IBUF:
                    buffer = clCreateBuffer(_context, CL_MEM_WRITE_ONLY | CL_MEM_HOST_READ_ONLY, sizeof(int) * size, NULL, &err);
                    break;
                case ArgTypes::IN_OUT_IBUF:
                    buffer = clCreateBuffer(_context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, sizeof(int) * size, value, &err);
                    break;
                case ArgTypes::IN_FBUF:
                    buffer = clCreateBuffer(_context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(float) * size, value, &err);
                    break;
                case ArgTypes::OUT_FBUF:
                    buffer = clCreateBuffer(_context, CL_MEM_WRITE_ONLY | CL_MEM_HOST_READ_ONLY, sizeof(float) * size, NULL, &err);
                    break;
                case ArgTypes::IN_OUT_FBUF:
                    buffer = clCreateBuffer(_context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, sizeof(float) * size, value, &err);
                    break;
                default:
                    buffer = 0;
                    err = CL_SUCCESS;
            }
            checkError(err, "clCreateBuffer");
            _buffers.push_back(buffer);
        }
    }
    catch (...) {
        freeBuffers();
        throw;
    }
}

void  OpenCL::writeBuffer(int bufId, void* data, size_t size)
{
    cl_int err = clEnqueueWriteBuffer(_queue, _buffers[bufId], CL_TRUE,  0, size, data, 0, NULL, NULL);
    checkError(err, "clCreateBuffer");
}


//~~~~~ Read buffers after kernel execution ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void OpenCL::readBuffers(std::vector<std::tuple<ArgTypes, void*, size_t>> args) 
{
    cl_int err;
    for (int index = 0; index < args.size(); index++)
    {
        auto arg = args[index];
        ArgTypes type = std::get<0>(arg);
        void* value = std::get<1>(arg);
        size_t size = std::get<2>(arg);

        switch (type) {
            case ArgTypes::OUT_IBUF:
            case ArgTypes::IN_OUT_IBUF:
                err = clEnqueueReadBuffer(_queue, _buffers[index], CL_TRUE, 0, sizeof(int) * size, value, 0, NULL, NULL);
                break;
            case ArgTypes::OUT_FBUF:
            case ArgTypes::IN_OUT_FBUF:
                err = clEnqueueReadBuffer(_queue, _buffers[index], CL_TRUE, 0, sizeof(float) * size, value, 0, NULL, NULL);
                break;
            default:
                err = CL_SUCCESS;
        }

        checkError(err, "clEnqueueReadBuffer");
    }
}

//~~~~~ Free OpenCL buffers ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void OpenCL::freeBuffers() {
    for (auto buffer : _buffers) if (buffer) clReleaseMemObject(buffer);
    _buffers.clear();
}

/**************************************************************************************************
 * OpenCL kernel execution
 *
 * This section contains functions to run the OpenCL kernel with the specified arguments.
 * The kernel is executed with the given global and local sizes.
 *
 **************************************************************************************************/

void OpenCL::runKernel(int idKkernel, std::vector<std::tuple<ArgTypes, void*, size_t>> args, const std::vector<size_t>& globalSize, const std::vector<size_t>& localSize) 
{
    cl_int err;
    cl_kernel kernel = _kernels[idKkernel];
    if (!kernel) throw OpenClError("Kernel not found");

    // Set kernel arguments

    for (int index = 0; index < args.size(); index++)
    {
        auto arg = args[index];
        ArgTypes type = std::get<0>(arg);
        void* value = std::get<1>(arg);
        size_t size = std::get<2>(arg);

        switch (type)
        {
            case ArgTypes::INT:
                err = clSetKernelArg(kernel, index, sizeof(int), value);
                break;
            case ArgTypes::IN_IBUF:
            case ArgTypes::OUT_IBUF:
            case ArgTypes::IN_OUT_IBUF:
            case ArgTypes::IN_FBUF:
            case ArgTypes::OUT_FBUF:
            case ArgTypes::IN_OUT_FBUF:
                err = clSetKernelArg(kernel, index, sizeof(cl_mem), &_buffers[index]);
                break;
            default:
                throw OpenClError("Invalid argument type");
        }
        checkError(err, "clSetKernelArg");
    }

    // Execute kernel

    cl_uint workDim = static_cast<cl_uint>(globalSize.size());
    size_t *workSize = new size_t[workDim]; 
    for (cl_uint i = 0; i < workDim; i++) workSize[i] = globalSize[i];
    size_t *groupSize = NULL;
    if (!localSize.empty())
    {
        groupSize = new size_t[workDim];
        for (cl_uint i = 0; i < workDim; i++) groupSize[i] = localSize[i];
    }
    err = clEnqueueNDRangeKernel(_queue, kernel, workDim, NULL, workSize, groupSize, 0, NULL, NULL);
    delete[] workSize;
    if (groupSize) delete[] groupSize;
    checkError(err, "clEnqueueNDRangeKernel");
}

/**************************************************************************************************

* OpenCL run method
*
* This method executes the full job circle: create buffers, run kernel, read results.
* It takes a vector of arguments, global size, and local size as input.
*
**************************************************************************************************/

void OpenCL::run(
    std::vector<std::tuple<ArgTypes, void*, size_t>> args,
    const std::vector<size_t>& globalSize,
    const std::vector<size_t>& localSize
)
{
    try {
        createBuffers(args);
        for (int k = 0; k < _kernels.size(); k++) runKernel(k, args, globalSize, localSize);
        readBuffers(args);
        freeBuffers();
    }
    catch (...) {
        freeBuffers();
        throw;
    }
}
