// OpenCL kernel for Gaussian elimination

// #pragma OPENCL EXTENSION cl_khr_fp64 : enable

__kernel void toTriangular(
    __global float *m, 
    __global float *result, 
    const int dim
) {
    __local float ratio;
    __local int w;

    int i = get_global_id(0);
    int j = get_global_id(1);
    int k = get_local_id(2);

    if ( k == 0) {
        w = dim + 1;
        ratio = m[j * w + i] / m[i * w + i];
    }
    barrier(CLK_LOCAL_MEM_FENCE);
    if (k == i) m[j * w + k] = 0;
    else if (k > i) m[j * w + k] -= ratio * m[i * w + k];
    barrier(CLK_GLOBAL_MEM_FENCE);

    // printf("%d %d %d\n", r, c, k);
}