#ifndef MATRIX_HPP
#define MATRIX_HPP

#include <iostream>
#include <vector>
#include <stdexcept>
#include <cmath>
#include <complex>
#include <map>
#include "Utils.hpp"


namespace algebra {

    template <typename T, StorageOrder order>
    class Matrix {
    private:
        std::map<Key, T, Compare> data;
        std::size_t rows, cols;
        bool compressed;
        // compressed format
        std::vector<T> values;
        std::vector<std::size_t> row_indices;
        std::vector<std::size_t> col_indices;

    public:
        //CONSTUCTORS
        
        //empty constructor
        Matrix(std::size_t rows, std::size_t cols) : rows(rows), cols(cols) {
            // No filling needed for a sparse matrix
        }

        //uncompressed constructor
        Matrix(std::map<Key, T, Compare> data, std::size_t rows, std::size_t cols) : 
            data(data), 
            rows(rows), 
            cols(cols),
            compressed(false) {};

        //compressed constructor
        Matrix(std::vector<T> values, std::vector<std::size_t> row_indices, std::vector<std::size_t> col_indices, std::size_t rows, std::size_t cols) : 
            values(values), 
            row_indices(row_indices), 
            col_indices(col_indices),
            rows(rows), 
            cols(cols),
            compressed(true) {};

        void resize(std::size_t rows, std::size_t cols) {
            this->rows = rows;
            this->cols = cols;
        }

        void compress() {
            if (is_compressed()) {
                return;  // Already compressed
            }

            if constexpr (order == ROW_MAJOR) {
                compressRowMajor();
            } else if constexpr (order == COL_MAJOR) {
                compressColMajor();
            }

            // Clear the uncompressed format map
            data.clear();

            compressed = true;
        }

        void uncompress() {
            if (!is_compressed()) {
                return;  // Already uncompressed
            }

            if constexpr (order == ROW_MAJOR) {
                uncompressRowMajor();
            } else if constexpr (order == COL_MAJOR) {
                uncompressColMajor();
            }

            // Clear the compressed format vectors
            values.clear();
            row_indices.clear();
            col_indices.clear();

            compressed = false;
        }

        bool is_compressed() const {
            return compressed;
        }

        T operator()(std::size_t i, std::size_t j) const {
            try {
                // Check if the matrix is compressed
                if (compressed) {
                    // Check the storage order of the matrix
                    if constexpr (storage_order == StorageOrder::ROW_MAJOR) 
                        return findElementRowMajor(i, j);
 
                    } else 
                        return findElementColMajor(i, j);

                } else {
                    // If the matrix is not compressed, return a reference to the element in the data map
                    return data.at(std::make_pair(i, j));
                }
            catch (const std::runtime_error& e) {
                std::cerr << e.what() << '\n'; 
            }
        }

        T& operator()(std::size_t i, std::size_t j) {
            try {
                // Check if the matrix is compressed
                if (compressed) {
                    // Check the storage order of the matrix
                    if constexpr (storage_order == StorageOrder::ROW_MAJOR) 
                        return findElementRowMajor(i, j);
 
                    } else 
                        return findElementColMajor(i, j);

                } else {
                    // If the matrix is not compressed, return a reference to the element in the data map
                    //here we check if i and j are greater than the rows and cols, in that case we resize the matrix
                    rows = i+1 > rows ? i+1 : rows;
                    cols = j+1 > cols ? j+1 : cols;
                    return data[std::make_pair(i, j)];
                }
            catch (const std::runtime_error& e) {
                std::cerr << e.what() << '\n'; 
            }
        }

        friend std::vector<T> operator*(const Matrix& m, const std::vector<T>& v) {
            if (!_is_compressed) {
                return _matrix_vector_uncompressed(vec);
            }
            if constexpr (Store == StorageOrder::row) {
                return _matrix_vector_row(vec);
            }
            return _matrix_vector_col(vec);
        };

        // extra
        friend Matrix operator*(const Matrix& m1, const Matrix& m2) {
            // ...
        }

        private:
            void compressRowMajor() {
                if (is_compressed())
                {
                    return;
                }
                std::size_t num_non_zero = 0;
                std::transform(data.begin(), data.end(), 
                    std::back_inserter(values), [&](const auto& pair) { //all local variables accessible
                    const auto& [k, v] = pair;
                    col_indices[num_non_zero] = k[1];
                    row_indices[k[0] + 1] = ++num_non_zero; //increment the row index
                    return v; //this v is appended to values
                    }
                );

                // finalizing
                compressed = true;
                data.clear();
            }

            void compressColMajor() {
                if (is_compressed())
                {
                    return;
                }
                std::size_t num_non_zero = 0;
                std::transform(data.begin(), data.end(), 
                    std::back_inserter(values), [&](const auto& pair) { //all local variables accessible
                    const auto& [k, v] = pair;
                    row_indices[num_non_zero] = k[0];
                    col_indices[k[1] + 1] = ++num_non_zero; //increment the row index
                    return v; //this v is appended to values
                    }
                );

                // finalizing
                compressed = true;
                data.clear();

            }

            void uncompressRowMajor() {
                if (!is_compressed())
                {
                    return;
                }
                
                std::size_t num_non_zero = 0;
                std::transform(values.begin(), values.end(), 
                    std::back_inserter(data), [&](const auto& v) { //all local variables accessible
                    const Key k = {row_indices[num_non_zero], col_indices[num_non_zero]};
                    ++num_non_zero;
                    return std::make_pair(k, v); //this v is appended to values
                    }
                );

                // finalizing
                compressed = false;
            }

            void uncompressColMajor() {
                if (!is_compressed())
                {
                    return;
                }
                std::size_t num_non_zero = 0;
                std::transform(values.begin(), values.end(), 
                    std::back_inserter(data), [&](const auto& v) { //all local variables accessible
                    const Key k = {row_indices[num_non_zero], col_indices[num_non_zero]};
                    ++num_non_zero;
                    return std::make_pair(k, v); //this v is appended to values
                    }
                );

                // finalizing
                compressed = false;
            }


            //non const and const methods for retrieving elements in compressed matrix
            T& findElementRowMajor(std::size_t row, std::size_t col) {
                auto start = values.begin() + row_indices[row];
                auto end = values.begin() + row_indices[row + 1];

                auto it = std::find_if(start, end, [&](const auto& value) {
                    std::size_t col_idx = &value - &values[0]; //index of the value in values
                    return col_indices[col_idx] == col;
                });

                if (it != end) {
                    return *it;
                }
                else {
                    return 0;
                }
            }

            T& findElementColMajor(std::size_t row, std::size_t col) {
                auto start = values.begin() + col_indices[col];
                auto end = values.begin() + col_indices[col + 1];

                auto it = std::find_if(start, end, [&](const auto& value) {
                    std::size_t row_idx = &value - &values[0]; //index of the value in values
                    return row_indices[row_idx] == row;
                });

                if (it != end) {
                    return *it;
                }
                else {
                    return 0;
                }
            }

            T findElementRowMajor(std::size_t row, std::size_t col) const{
                auto start = values.begin() + row_indices[row];
                auto end = values.begin() + row_indices[row + 1];

                auto it = std::find_if(start, end, [&](const auto& value) {
                    std::size_t col_idx = &value - &values[0]; //index of the value in values
                    return col_indices[col_idx] == col;
                });

                if (it != end) {
                    return *it;
                }
                else {
                    return 0;
                }
            }

            T findElementColMajor(std::size_t row, std::size_t col) const{
                auto start = values.begin() + col_indices[col];
                auto end = values.begin() + col_indices[col + 1];

                auto it = std::find_if(start, end, [&](const auto& value) {
                    std::size_t row_idx = &value - &values[0]; //index of the value in values
                    return row_indices[row_idx] == row;
                });

                if (it != end) {
                    return *it;
                }
                else {
                    return 0;
                }
            }

            std::vector<T> mat_vec_prod_uncompressed(std::vector<T> vec) const {
                std::vector<T> out(rows, 0);
                std::transform(data.begin(), data.end(), out.begin(),
                    [&vec](const auto& kv) { return vec[kv.first[1]] * kv.second; },
                    std::plus<>());

                return out;
            }


    };
}


#endif // MATRIX_HPP