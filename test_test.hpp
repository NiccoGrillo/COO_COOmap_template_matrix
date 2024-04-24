#include <iostream>
#include <string>
#include "Matrix.hpp"
#include "ReadMatrix.hpp"
#include "Utils.hpp"
#include "chrono.hpp"

using namespace algebra;
template<typename T, StorageOrder Order>
class MatrixTest {
public:

    MatrixTest(const std::string& file_name) {
      matrix1 = Matrix<T, Order>(file_name);
      matrix2 = Matrix<T, Order>(file_name);
    }
    void runTests() {
    testFileReader();
    testBasicOperations();
    testMultiplicationCorrectness();
    testNorm();
    benchmarkMultiplication();
    largeBenchmarkMultiplication();
    }

private:
    void TestReadMatricesAndConstructMembers() {
    std::cout << "Testing Reader Constructor and filling matrix ...\n";

    
    }
    void constructMatrix

    void testBasicOperations() {
    std::cout << "Running test_basic_operations...\n";
    // TODO: Add test cases for the basic operations method
    }

    void testMultiplicationCorrectness() {
    std::cout << "Running test_multiplication_correctness...\n";
    // TODO: Add test cases for the multiplication correctness method
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

  //empty Matrix as private member
    Matrix<T, Order> matrix1;
    Matrix<T, Order> matrix2; //this will be needed for multiplication tests


};
