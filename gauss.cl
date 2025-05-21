// OpenCL kernel for Gaussian elimination

// #pragma OPENCL EXTENSION cl_khr_fp64 : enable // Uncomment if using printf

// One step of forward elimination

__kernel void zeroOutCol(
    __global float *m, 
    __global float *result, 
    __global float *errors, 
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
}

// One step of backward substitution

__kernel void calcRoot(
    __global float *m, 
    __global float *result, 
    __global float *errors, 
    const int col
){
    __local int w, d;
    __local float r;

    int row = get_local_id(0);

    if (row == col) {
        d = get_global_size(0);
        w = d + 1;
        r = m[row * w + d] / m[row * w + row];
        result[row] = r;
    }
    
    barrier(CLK_LOCAL_MEM_FENCE);

    if (row < col) {
        m[row * w + d] -= m[row * w + col] * r;
    }
}

// Calculate the error for each row

__kernel void calcError(
    __global float *m, 
    __global float *result,
    __global float *errors, 
    const int col
){

    __local int w, d;
    __local float r;

    int row = get_local_id(0);

    if (row == 0) {
        d = get_global_size(0);
        w = d + 1;
        r = result[col];
        errors[row] = m[row * w + d];
    }
    
    barrier(CLK_LOCAL_MEM_FENCE);

    errors[row] -= m[row * w + col] * r;
}
