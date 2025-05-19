// OpenCL kernel for Gaussian elimination

// #pragma OPENCL EXTENSION cl_khr_fp64 : enable

__kernel void zeroOutCol(
    __global float *m, 
    __global float *result, 
    // const int dim,
    const int col
) {
    __local float ratio;
    __local int w, d;

    int j = get_global_id(0);
    int k = get_local_id(1);

    if (k == 0) {
        d = get_global_size(0);
        w = d + 1;
        ratio = m[j * w + col] / m[col * w + col];
    }
    barrier(CLK_LOCAL_MEM_FENCE);

    if (j > col && j < d) {
        if (k == col) m[j * w + k] = 0;
        else if (k > col && k <= d) m[j * w + k] -= ratio * m[col * w + k];
    }
    //barrier(CLK_GLOBAL_MEM_FENCE);

    // printf("%d %d %d\n", r, c, k);
}