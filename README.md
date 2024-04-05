### Setareh Nouri - A01308077

# Assignment 3

## Overview
This is a simple memory allocator implemented in C. It provides functions for memory initialization, allocation, and reallocation. The allocator is designed to manage a contiguous region of memory and allocate memory blocks of variable sizes.

## Features
- **Dynamic Memory Allocation**: The allocator can dynamically allocate memory blocks of varying sizes.
- **Efficient Memory Management**: It efficiently manages memory by splitting and merging free blocks as needed.
- **Alignment**: Memory allocations are aligned to 8-byte boundaries.

## Usage
### Initialization
To use the memory allocator in your C program, include the `"mem.h"` header file in your source code and initialize the allocator with a region size using the `mem_init` function.

### Allocation
Allocate memory using the `mem_alloc` function.

### Deallocation
Free allocated memory using the `mem_free` function when it's no longer needed.



