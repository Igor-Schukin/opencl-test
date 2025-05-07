// OpenCL kernel for adding two integer arrays

__kernel void array_add(
    __global const int *a, 
    __global const int *b,
    __global int *result, 
    const int size
) {
    int idx = get_global_id(0); // Get the global thread index
    if (idx < size) {
        result[idx] = a[idx] + b[idx]; // Add elements
    }
}