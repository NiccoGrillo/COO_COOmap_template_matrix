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
            // ...
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
            // ...
        }

        T& operator()(std::size_t i, std::size_t j) {
            // ...
        }

        friend std::vector<T> operator*(const Matrix& m, const std::vector<T>& v) {
            // ...
        }

        // extra
        friend Matrix operator*(const Matrix& m1, const Matrix& m2) {
            // ...
        }

        private:
            void compressRowMajor() {

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

    };
}


#endif // MATRIX_HPP