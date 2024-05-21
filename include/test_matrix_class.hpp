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
  MatrixTest(int verbose = 0, int num_runs = 0) : verbose(verbose), num_runs(num_runs) {};
  
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

    //decompressing the matrix for the next tests
    matrix1.uncompress();
}

  void testVectorMultiplication(int num_runs = 0) {
    std::cout << "Running test_vector_multiplication...\n";
      Timings::Chrono timer;

      std::cout << "Testing correctness of the multiplication for the UNCOMPRESSED matrix:\n";
      std::vector<T> vec = vector_generator<T>(matrix1.get_cols());
      // std::vector<T> vec(matrix1.get_cols(), 1.0);  // -> for manual testing
      auto out_vec = matrix1 * vec;
      if (verbose != 0)
      {
        std::cout << "The result of the multiplication is: \n";
        int count  = 0;
        for (auto i : out_vec)
        {
          std::cout <<"[" << count << "]  ->" << i << "\n ";
          count++;
        }

        
      }      
      std::cout << "Multiplication done correctly\n";
      std::cout << "\n\n";

      std::cout << "Testing correctness of the multiplication for the COMPRESSED matrix:\n";
      matrix1.compress();
      auto out_vec2 = matrix1 * vec;
      if (verbose != 0)
      {
        std::cout << "The result of the multiplication is: \n";
        int count  = 0;
        for (auto i : out_vec2)
        {
          std::cout <<"[" << count << "]  ->" << i << " \n";
          count++;
        }
        std::cout << "\n\n";
      }
      bool same = (out_vec == out_vec2);
      std::cout << "Are the two results the same? " << (same ? "YES" : "NO") << "\n";
      if (same)
        std::cout << "Multiplication done correctly\n";
      else
      {
        std::cout << "TEST FAILED. The multiplication is incorrect\n";
        return;
      }
      std::cout << "\n\n";


      // //uncompressing the matrix for the benchmarking
      matrix1.uncompress();


      if (num_runs >= 1){
        std::cout << "Benchmarking the multiplication and compression by running "<< num_runs << "runs\n";
        double time_prod_uncop = 0.0;
        double time_prod_comp = 0.0;
        double time_compression = 0.0;
        double time_uncompression = 0.0;

        for (int i = 0; i < num_runs; ++i) {
          std::vector<T> vec = vector_generator<T>(matrix1.get_cols());

          timer.start();
          auto out = matrix1 * vec;
          timer.stop();
          time_prod_uncop += timer.wallTime();

          //compression
          timer.start();
          matrix1.compress();
          timer.stop();
          time_compression += timer.wallTime();

          timer.start();
          auto out_compressed = matrix1 * vec;
          timer.stop();
          time_prod_comp += timer.wallTime();

          //uncompression
          timer.start();
          matrix1.uncompress();
          timer.stop();
          time_uncompression += timer.wallTime();

        }

        // Calculate average times
        double avg_time_prod_uncop = time_prod_uncop / num_runs;
        double avg_time_prod_comp = time_prod_comp / num_runs;
        double avg_time_compression = time_compression / num_runs;
        double avg_time_uncompression = time_uncompression / num_runs;

        std::cout << "Average time for UNCOMPRESSED multiplication: " << avg_time_prod_uncop << " micro seconds\n";
        std::cout << "Average time for COMPRESSED multiplication: " << avg_time_prod_comp << " micro seconds\n";
        std::cout << "Average time for compression: " << avg_time_compression << " micro seconds\n";
        std::cout << "Average time for uncompression: " << avg_time_uncompression << " micro seconds\n\n";
      }
      std::cout << "Multiplication tests passed\n";
      std::cout << "--------------------------------\n";

  }

  template <WhichNorm NORM>
  void testNorm(int num_runs = 0) {
    Timings::Chrono timer;

    std::cout << "Running test_norm...\n";
    std::cout << "Testing correctness of the norm for the UNCOMPRESSED matrix: " <<  "\n";
    std::cout << "The frobenius norm of the matrix is: " << matrix1.template norm<WhichNorm::FROBENIUS>() << "\n";
    //now max norm
    std::cout << "The max norm of the matrix is: " << matrix1.template norm<WhichNorm::MAX>() << "\n";
    //now one norm
    matrix1.uncompress();
    std::cout << "The one norm of the matrix is: " << matrix1.template norm<WhichNorm::ONE>() << "\n\n";

    matrix1.compress();
    std::cout << "Testing correctness of the norm for the COMPRESSED matrix:" << "\n";
    std::cout << "The frobenius norm of the matrix is: " << matrix1.template norm<WhichNorm::FROBENIUS>() << "\n";
    std::cout << "The max norm of the matrix is: " << matrix1.template norm<WhichNorm::MAX>() << "\n";
    std::cout << "The one norm of the matrix is: " << matrix1.template norm<WhichNorm::ONE>() << "\n\n";

    //uncompressing the matrix for the benchmarking
    matrix1.uncompress();

  if (num_runs >= 1){
      std::cout << "Benchmarking the multiplication and compression by running "<< num_runs << "runs\n";
      std::array<double, 3> time_norm_uncop = {0.0, 0.0, 0.0};
      std::array<double, 3> time_norm_comp = {0.0, 0.0, 0.0};
      std::array<std::string, 3> norm_names = {"FROBENIUS", "ONE", "MAX"};

      for (int i = 0; i < num_runs; ++i) {
          std::vector<T> vec = vector_generator<T>(matrix1.get_cols());

          timer.start();
          matrix1.template norm<WhichNorm::FROBENIUS>();
          timer.stop();
          time_norm_uncop[0] += timer.wallTime();

          timer.start();
          matrix1.template norm<WhichNorm::ONE>();
          timer.stop();
          time_norm_uncop[1] += timer.wallTime();

          timer.start();
          matrix1.template norm<WhichNorm::MAX>();
          timer.stop();
          time_norm_uncop[2] += timer.wallTime();

          matrix1.compress();

          timer.start();
          matrix1.template norm<WhichNorm::FROBENIUS>();
          timer.stop();
          time_norm_comp[0] += timer.wallTime();

          timer.start();
          matrix1.template norm<WhichNorm::ONE>();
          timer.stop();
          time_norm_comp[1] += timer.wallTime();

          timer.start();
          matrix1.template norm<WhichNorm::MAX>();
          timer.stop();
          time_norm_comp[2] += timer.wallTime();

          matrix1.uncompress();
      }

      // Calculate average times and print results
      for (int norm_type = 0; norm_type < 3; ++norm_type) {
          double avg_time_norm_uncop = time_norm_uncop[norm_type] / num_runs;
          double avg_time_norm_comp = time_norm_comp[norm_type] / num_runs;
          std::cout << "Norm: " << norm_names[norm_type] << "\n";
          std::cout << "Average time uncompressed: " << avg_time_norm_uncop << "\n";
          std::cout << "Average time compressed: " << avg_time_norm_comp << "\n";
      }
  }
  std::cout << "Norm tests passed\n";
  std::cout << "--------------------------------\n\n";
  }

  //generate random matrix
private:
  //empty Matrix as private member
    Matrix<T, order> matrix1;
    Matrix<T, order> matrix2; //this will be needed for multiplication tests
    int verbose;
    int num_runs;

};

#endif
