# Examples of OpenCL Usage on the RTU HPC Platform

## OpenCL Verification
1. Register in the RTU HPC system
   ```
   ssh <user-name>@ui-2.hpc.rtu.lv
   ```
1. Clone the project if it hasn't been cloned yet
   ```
   git clone git@github.com:Igor-Schukin/opencl-test.git
   ```
1. Navigate to the `opencl-test` folder with test
   ```
   cd opencl-test\cltest
   ```
1. Compile and run the test
   ```
   .\run.sh
   ```
1. After the test is completed (`Bye` should appear on the screen), end the program by pressing `Ctrl+C`
1. The test results can be seen on the screen and in the `cltest.out` file
   ```
   cat cltest.out
   ```

## OpenCL Usage Examples
1. Register in the RTU HPC system
   ```
   ssh <user-name>@ui-2.hpc.rtu.lv
   ```
1. Clone the project if it hasn't been cloned yet
   ```
   git clone git@github.com:Igor-Schukin/opencl-test.git
   ```
1. Navigate to the `opencl-test` folder
   ```
   cd opencl-test
   ```
1. Compile and run one of the examples
   ```
   .\run.sh <example-name>
   ```
1. After the example program is completed (`Bye` should appear on the screen), end it by pressing `Ctrl+C`
1. The results can be seen on the screen and in the `<example-name>.out` file
   ```
   cat <example-name>.out
   ```

## Implemented Examples
The project includes the following examples of OpenCL usage for data processing:
1. ***sum*** - an example of parallel computation of the sum of two vectors `a` and `b`:
   - the vectors have 100 million elements, filled with integers: `a[i] = 2i`, `b[i] = -i`;  
   - as a result, the sum vector should contain integers `0, 1, 2...`;  
   - for verification, the first 10 elements of the resulting vector are displayed on the screen;  
   - the times required for computation using the GPU and the CPU are measured.