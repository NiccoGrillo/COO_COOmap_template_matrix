#include <iostream>
#include <string>

#include "Matrix.hpp"
// #include "chrono.hpp"
#include "Utils.hpp"
#include "test_matrix_class.hpp"

using namespace algebra;

int main(int argc, char* argv[]) {
  using type_chosen = std::complex<double>;
  std::string big_file_name = "./lnsp_131.mtx";
  std::string small_file_name = "./small_example.mtx";

  // Parse command line arguments
  int verbose = 0; //do we want to print the matrices?
  int num_runs = 0; // do we want to test the performance?
  for (int i = 1; i < argc; i++) {
    std::string arg = argv[i];
    if (arg == "--verbose") {
      if (i + 1 < argc) {
        verbose = std::stoi(argv[i + 1]);
        i++; 
      } else {
        std::cerr << "Error: --verbose option requires an argument" << std::endl;
        return 1;
      }
    } else if (arg == "--num_runs") {
      if (i + 1 < argc) {
        num_runs = std::stoi(argv[i + 1]);
        i++; 
      } else {
        std::cerr << "Error: --num_runs option requires an argument" << std::endl;
        return 1;
      }
    }
  }

  //choose the type of the matrix here:
  MatrixTest<type_chosen, StorageOrder::ROW_MAJOR> tester(verbose, num_runs); // 0 for no print of the whole matrices

  // Test the reader - initialize tester matrix member
  tester.ReadMatrices(big_file_name, 1);  // file_matrix_name, which matrix to read 

  // Test if compression and uncompression work
  tester.testCompressionUncompression();


  // // Test the const and non-const call operator
  tester.testCallOperator(1,3,10); // 1,3 is not present in the matrix
  tester.ReadMatrices(big_file_name, 1); //reset the matrix to previous state

  // // Test the matrix-vector multiplication
  tester.testVectorMultiplication();  //note: in the previous test we changed the dimension of the matrix

  // Test the norm
  tester.testNorm<WhichNorm::FROBENIUS>();

  return 0;
}
