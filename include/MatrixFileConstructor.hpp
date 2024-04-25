#ifndef MATRIX_FILE_CONSTRUCTOR_HPP
#define MATRIX_FILE_CONSTRUCTOR_HPP

#include <array>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>

#include "Matrix.hpp"
#include "Utils.hpp"
using namespace algebra;

namespace algebra {

/**
 * @brief Read a matrix in the matrix-market format.
 *
 * @tparam T Type of the matrix entries.
 * @tparam Store StorageOrder for the matrix, deciding the ordering of the
 * mapping.
 * @param file_name Path to the matrix-market file.
 * @return std::map<std::array<std::size_t, 2>, T,
 * std::conditional_t<Store == StorageOrder::row, RowOrderComparator<T>,
 * ColOrderComparator<T>>> Mapping "(row, col) -> value" which can be directly
 * passed into the constructor.
 */
template<Numeric T, StorageOrder order>
Matrix<T, order>::Matrix(const std::string& file_name) {
  using MapType = std::map<Key, T,  Compare<T, order>>;

  std::ifstream file(file_name);
  if (!file.is_open()) {
    throw std::runtime_error("Failed to open file: " + file_name);
  }

  std::string line;

  while (file.peek() == '%') {
    file.ignore(2048, '\n');
  }
  // std::istringstream iss(line);
  std::size_t num_rows, num_cols, num_elements;

  file >> num_rows >> num_cols >> num_elements;

  MapType entry_value_map;

  for (std::size_t i = 0; i < num_elements; ++i) {
    std::size_t row, col;
    T value;
    file >> row >> col >> value;

    // we always use the format (row, col) -> value
    // only the comparison operator is different
    entry_value_map[{row - 1, col - 1}] = value;
  }
  //finally we delegate the constructor
  data = entry_value_map;
  rows = num_rows;
  cols = num_cols;
  compressed = false;
}
}  // namespace algebra

#endif