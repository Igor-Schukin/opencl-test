#ifndef OPENCL_H
#define OPENCL_H

#include <CL/cl.h>
#include <stdexcept>
#include <string>
#include <vector>
#include <tuple>

enum class ArgTypes { INT, IN_IBUF, OUT_IBUF };

size_t getTime();

class OpenClError : public std::runtime_error {
public:
    OpenClError(cl_int err, const std::string& operation);
    OpenClError(const std::string& message);
};

class OpenCL {
private:
    cl_platform_id _platform = 0;
    cl_device_id _device = 0;
    cl_context _context = nullptr;
    cl_command_queue _queue = nullptr;
    cl_program _program = nullptr;
    cl_kernel _kernel = nullptr;

    char* loadKernelSource(const std::string& filename);
    void checkError(cl_int err, const std::string& operation);
    void init(const std::string& kernelSourceFile, const std::string& kernelName);
    void release();

public:
    OpenCL(const std::string& kernelSourceFile, const std::string& kernelName);
    ~OpenCL();

    void run(std::vector<std::tuple<ArgTypes, void*, size_t>> args, size_t dimSize, cl_uint workDim  = 1);
};

#endif // OPENCL_H
