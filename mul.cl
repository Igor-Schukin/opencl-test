// OpenCL kernel for multiplying two integer matrix

__kernel void mul(
    __global const int *a, 
    __global const int *b,
    __global int *result, 
    const int dim
) {
    int r = get_global_id(0);
    int c = get_global_id(1);
    int k = get_global_id(2);
    if (r < dim && c < dim && k < dim) {
        result[r * dim + c] += a[r * dim + k] * b[k * dim + c];
    }
}