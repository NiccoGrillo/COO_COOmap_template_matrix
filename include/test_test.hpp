#ifndef TEST_TEST_HPP
#define TEST_TEST_HPP

#include <iostream>
#include <string>
#include "Matrix.hpp"
#include "MatrixFileConstructor.hpp"
#include "Utils.hpp"
#include "chrono.hpp"

using namespace algebra;
template<Numeric T, StorageOrder order>
class MatrixTest {
public:
  MatrixTest(int verbose = 1) : verbose(verbose) {};
  
  void ReadMatrices(const std::string& file_name, int num_mat = 1) {
    
    if (/* condition */ num_mat == 1)
    {
      
      matrix1 = Matrix<T, order>(file_name);
      // matrix2 = Matrix<T, Order>;
    }
    else if (num_mat == 2)
    {
      matrix2 = Matrix<T, order>(file_name);
    }
    std::cout << "Matrix(ces) created correctly\n";
    if (verbose != 0)
    {
      std::cout << matrix1;
    }
    std::cout << "--------------------------------\n";
  }
  
  void runTests() {
  testCompressionUncompression();
  testCallOperator();
  testNorm();
  benchmarkMultiplication();
  largeBenchmarkMultiplication();
  }

  void testCompressionUncompression() {
    std::cout << "Running test_compression_uncompression...\n";
    matrix1.compress();
    std::cout << "\n \n Matrix compressed: \n";
    if (verbose != 0)
    {
      std::cout << matrix1;

    }
    std::cout << "\n \n Matrix decompressed: \n \n";
    matrix1.uncompress();
    if (verbose != 0)
    {
      std::cout << matrix1;
    }
    std::cout << "Compression and uncompression tests passed\n";
    std::cout << "--------------------------------\n";
    }

  void testCallOperator(std::size_t row1, std::size_t col1, T new_val) {
    // Create a Matrix object
    if (!matrix1.is_inside(row1, col1)){
      std::cout << "The indices are out of bounds, please pick them inside the bounds. The test is already incorporating the out of bounds coordinates\n";
      return;
    }

    std::cout << "Running test_call_operator...\n";
    std::cout << "the dimensions are: " << matrix1.get_rows() << " x " << matrix1.get_cols() << "\n\n";
    

    std::cout<<"old value at (" << row1 << ", " << col1 << "): " << matrix1(row1, col1) << "\n";
    matrix1(row1, col1) = new_val;
    std::cout << "After setting value at (" << row1 << ", " << col1 << "): " << matrix1(row1, col1) << "\n\n";

  
    std::cout << "testing const call operator\n";
    const Matrix<T, order> const_copy = matrix1;
    std::cout << "Value at (" << row1 << ", " << col1 << ") in const matrix copy: " << const_copy(row1, col1) << "\n\n";

    std::cout << "testing non-const call operator with out-of-bounds indices\n";
    try {
        matrix1(matrix1.get_rows() + 100, matrix1.get_cols() + 100) = new_val * static_cast<T>(3);
        std::cout << "After setting value at out-of-bounds indices in compressed matrix1: " << matrix1(row1 + 100, col1 + 100) << std::endl;
        std::cout <<"New dimensions are: " << matrix1.get_rows() << " x " << matrix1.get_cols() << "\n\n\n";
    } catch (const std::runtime_error& e) {
        std::cout << "TEST FAILED. Caught out-of-range error, this should not happen: " << e.what() << std::endl;
        return;
    }


    std::cout << "testing for COMPRESSED matrix: \n";
    matrix1.compress();
    // Test the non-const call operator of the compressed matrix1
    matrix1(row1, col1) = new_val * static_cast<T>(2);
    std::cout << "After setting value (double of before) at (" << row1 << ", " << col1 << ") in compressed matrix: " << matrix1(row1, col1) << "\n\n";

    // Test the const call operator of the compressed matrix1
    std::cout << "testing const call operator\n";
    const Matrix<T, order> const_compressed_copy = matrix1;
    std::cout << "Value at (" << row1 << ", " << col1 << ") in const compressed matrix1: " << const_compressed_copy(row1, col1) << "\n\n";

    // Test the const call operator with out-of-bounds indices
    std::cout << "testing const call operator with out-of-bounds indices\n";
    try {
        std::cout << "Value at out-of-bounds indices in const compressed matrix1: " << const_compressed_copy(row1 + 100, col1 + 100) << std::endl;
    } catch (const std::runtime_error& e) {
        std::cout << "Caught out-of-range error as expected: " << e.what() << std::endl;
    }

    // Test the non-const call operator with out-of-bounds indices
    std::cout << "testing non-const call operator with out-of-bounds indices\n";
    try {
        matrix1(matrix1.get_rows() + 200, matrix1.get_cols() + 200) = new_val * static_cast<T>(3);
        std::cout << "After setting value at out-of-bounds indices in compressed matrix1: " << matrix1(row1 + 100, col1 + 100) << std::endl;
    } catch (const std::runtime_error& e) {
        std::cout << "Caught out-of-range error, as expected: " << e.what() << "\n\n";
    }
    std::cout << "Call operator tests passed\n";
    std::cout << "--------------------------------\n";
}

  void testVectorMultiplication() {
    std::cout << "Running test_vector_multiplication...\n";

  }

  void testNorm() {
  std::cout << "Running test_norm...\n";
  // TODO: Add test cases for the norm method
  }

  void benchmarkMultiplication() {
  std::cout << "Running benchmark_multiplication...\n";
  // TODO: Add benchmark test cases for the multiplication method
  }

  void largeBenchmarkMultiplication() {
  std::cout << "Running large_benchmark_multiplication...\n";
  // TODO: Add large scale benchmark test cases for the multiplication method
  }

  //generate random matrix
private:
  //empty Matrix as private member
    Matrix<T, order> matrix1;
    Matrix<T, order> matrix2; //this will be needed for multiplication tests
    int verbose;

};

#endif