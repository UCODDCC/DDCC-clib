# libddcc basic ussage
The library manual is at the project wiki

# Library installation
Clone the repository

`git clone https://github.com/UCODDCC/DDCC-clib`

`cd DDCC-clib`

build the library

`mkdir build && cd build`

`cmake ..`

`make`

Install the library

`sudo make install`

# How to implement the library
Use the library as any other library in your system
```C
/** main.c
 * This programs makes the matrix multiplication of a and b
 * and stores the result in the result variable
 */
#include <ddcc.h>
#include <stdio.h>

int main() {
    float **a, **b, **result;
    int x=5, y=5;

    a = (float**)malloc(sizeof(float*)*x);
    b = (float**)malloc(sizeof(float*)*x);
    result = (float**)malloc(sizeof(float*)*x);
    for (int i = 0; i < x; ++i) {
        a[i] = (float*)malloc(sizeof(float)*y);
        b[i] = (float*)malloc(sizeof(float)*y);
        result[i] = (float*)malloc(sizeof(float)*y);
        for (int j = 0; j < y; ++j) {
            a[i][j] = (float)i+j;
            b[i][j] = (float)i-j;
        }
    }

    ddccMatrixMultiplication(a, x, y, b, x, y, &result);
    return 0;
}
```

# Building the library
## gcc stand alone
`gcc main.c -o matrix.out -lddcc`
## CMake
```CMake
# Basic cmake file
cmake_minimum_required (VERSION 3.9)
project(matrix VERSION 1.0.0 DESCRIPTION "matrix multiplication ussing libddcc")
set(SOURCES main.c)
add_executable(${PROJECT_NAME} ${TARGET} ${SOURCES})

# Add the following line to your project to enable ucoddcc
target_link_libraries(${PROJECT_NAME} ddcc)
```