// OpenCL kernel for multiplying two integer matrix

// #pragma OPENCL EXTENSION cl_khr_fp64 : enable

__kernel void mul(
    __global const int *a, 
    __global const int *b,
    __global int *result, 
    const int dim
) {
    int r = get_global_id(0);
    int c = get_global_id(1);
    int k = get_global_id(2);
    // printf("%d %d %d\n", r, c, k);
    if (r < dim && c < dim && k < dim) {
        int v =  a[r * dim + k] * b[k * dim + c];
        atomic_add(&result[r * dim + c], v);
    }
}