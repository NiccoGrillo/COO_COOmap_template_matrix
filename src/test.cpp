#include <iostream>
#include <string>

#include "Matrix.hpp"
// #include "chrono.hpp"
#include "Utils.hpp"
#include "test_test.hpp"

using namespace algebra;

int main(int argc, char* argv[]) {
  using type_chosen = std::complex<double>;
  std::string big_file_name = "./lnsp_131.mtx";
  std::string small_file_name = "./small_example.mtx";

  std::string general_file = "";

  //choose the type of the matrix here:
  MatrixTest<type_chosen, StorageOrder::COL_MAJOR> tester(1); // 0 for no print of the whole matrices

  // Check if an argument was provided
  // if (argc > 1) {
  //   // Use the provided filename
  //   general_file = argv[1];
  // }

  // Test the reader - initialize tester
  tester.ReadMatrices(big_file_name, 1);  // file_matrix_name, which matrix to read 
  

  // Test if compression and uncompression work
  tester.testCompressionUncompression();
  // tester.testCompressionUncompression();

  // Test the call operator
  tester.testCallOperator(1,3, 20);

  // test the norm computation
  // test_norm<type_format, StorageOrder::row>(file_name);
  // test_norm<type_format, StorageOrder::col>(file_name);

  // Test if multiplication works
  // test_multiplication_correctness<type_format, StorageOrder::row>(file_name);
  // test_multiplication_correctness<type_format, StorageOrder::col>(file_name);

  // Benchmark test for the matrix-vector multiplication
  // benchmark_multiplication<type_format, StorageOrder::row>();
  // benchmark_multiplication<type_format, StorageOrder::col>();

  // Large benchmark test with lots of runs
  // large_benchmark_multiplication<type_format, StorageOrder::row>(1);
  // large_benchmark_multiplication<type_format, StorageOrder::col>(1);
  return 0;
}