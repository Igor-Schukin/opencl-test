#ifndef OPENCL_H
#define OPENCL_H

#include <CL/cl.h>
#include <stdexcept>
#include <string>
#include <vector>
#include <tuple>

enum class ArgTypes { INT, IN_IBUF, OUT_IBUF, IN_OUT_IBUF, IN_FBUF, OUT_FBUF, IN_OUT_FBUF };

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
    std::vector<cl_mem> _buffers{};

    char* loadKernelSource(const std::string& filename);
    void checkError(cl_int err, const std::string& operation);
    void init(const std::string& kernelSourceFile, const std::string& kernelName);
    void release();

public:
    OpenCL(const std::string& kernelSourceFile, const std::string& kernelName);
    ~OpenCL();

    void run(std::vector<std::tuple<ArgTypes, void*, size_t>> args, const std::vector<size_t>& globalSize, const std::vector<size_t>& localSize = {});

    void createBuffers(std::vector<std::tuple<ArgTypes, void*, size_t>> args);
    void readBuffers(std::vector<std::tuple<ArgTypes, void*, size_t>> args);
    void freeBuffers();
    void runKernel(std::vector<std::tuple<ArgTypes, void*, size_t>> args, const std::vector<size_t>& globalSize, const std::vector<size_t>& localSize = {});
};

#endif // OPENCL_H
