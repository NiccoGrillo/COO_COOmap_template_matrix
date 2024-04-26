
Second challenge/assignment of the PACS course (Advanced programming for scientific computing). 
Sure, here's a basic README file for your Matrix project:


# COO_CSR Template Matrix Project

This project provides a C++ implementation for matrix operations with a focus on performance. It includes functionality for different types of matrix norms and compression.

## Prerequisites

- A modern C++ compiler with support for C++20.
- `mk_modules` installed 
- GNU Make

## Building

The project uses a Makefile for building. To build the project, navigate to the project directory `/src` and run:

```bash
make
```

This will compile all the source files and generate the executable files.

## Usage

After building the project, you can run the executables. For example:

```bash
./test --num_runs 10 --verbose 1
```

This will run the executable with 10 runs and verbose output.

## Options

- `--num_runs`: Specifies the number of runs for the benchmark. Default is 0, so no testing.
- `--verbose`: Specifies the verbosity level. Default is 0, so the matrices are not printed (recommended).

## Cleaning Up

To clean up the build artifacts, run:

```bash
make clean
```

To also remove any generated documentation and temporary files, run:

```bash
make distclean
```

## Technical Notes

- The implementation is designed to work correctly with various data types including `double`, `int`, `float`, and `std::complex`.
- The following operations are supported:
  - Norm calculations: The `norm` method supports the Frobenius norm (`FROBENIUS`), 1-norm (`ONE`), and max norm (`MAX`).
  - Matrix-vector product: The `matrix_vector_product` method performs the multiplication of a matrix by a vector.
  - Compression and decompression: The `compress` and `uncompress` methods allow for efficient storage and retrieval of matrix data.
  - Indexing: General indexing operations are supported for accessing and modifying matrix elements.

## License

This project is thought to have no license.
```

Please replace "executable_name" with the actual name of your executable, and update the other sections as necessary to match your project's details.