#ifndef MATRIX_HPP
#define MATRIX_HPP

#include <iostream>
#include <vector>
#include <stdexcept>
#include <cmath>
#include <complex>
#include <map>
#include "Utils.hpp"
#include <iomanip>
#include <algorithm>

namespace algebra {

    template <Numeric T, StorageOrder order>
    class Matrix {
    private:
        std::map<Key, T, Compare<T, order>> data;
        std::size_t rows, cols;
        bool compressed;
        // compressed format
        std::vector<T> values;
        std::vector<std::size_t> row_indices;
        std::vector<std::size_t> col_indices;

    public:
        //CONSTUCTORS
        //default constructor
        Matrix() = default;
        //empty constructor
        Matrix(std::size_t rows, std::size_t cols) : rows(rows), cols(cols) {
            // No filling needed for a sparse matrix
        }

        //uncompressed constructor
        Matrix(std::map<Key, T, Compare<T, order>> data, std::size_t rows, std::size_t cols) : 
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

        //file reader constructor (defined in MatrixFileConstructor.hpp)
        Matrix(const std::string& file_name);

        void resize(std::size_t rows, std::size_t cols) {
            this->rows = rows;
            this->cols = cols;
        }

        std::size_t get_num_non_zero() const {
            if (compressed) {
                return values.size();
            }
            return data.size();
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
            if (i >= rows || j >= cols) 
                throw std::out_of_range("Row or column is outside the matrix");            
            // Check if the matrix is compressed
            if (compressed) {
                // Check the storage order of the matrix
                if constexpr (order == StorageOrder::ROW_MAJOR) 
                    return findElementRowMajor(i, j);

                 else 
                    return findElementColMajor(i, j);

            } else {
                 // If the matrix is not compressed, find the element in the data map
                auto it = data.find(Key{i, j});
                if (it != data.end())
                    return it->second;
                else 
                    return T();
            }                    
                    
 
        }

        T& operator()(std::size_t i, std::size_t j) {

            // Check if the matrix is compressed
            if (compressed) {
                // Check the storage order of the matrix
                if constexpr (order == StorageOrder::ROW_MAJOR) 
                    return findElementRowMajor(i, j);

                else 
                    return findElementColMajor(i, j);

            } else {
                // If the matrix is not compressed, return a reference to the element in the data map
                //here we check if i and j are greater than the rows and cols, in that case we resize the matrix
                rows = i+1 > rows ? i+1 : rows;
                cols = j+1 > cols ? j+1 : cols;
                return data[Key{i,j}];
            }
        }
        
        bool is_inside(std::size_t row, std::size_t col) const {
            if (row >= rows || col >= cols) {
                return false; // Indices are out of bounds
            }
            return true;
        }
    
        //norm
        template <WhichNorm NORM>
        T norm() const {
            if constexpr (NORM == WhichNorm::FROBENIUS) {
                if (!is_compressed()) {
                    return std::sqrt(std::accumulate(data.begin(), data.end(), 0.0,
                        [](double acc, const auto& pair) {
                            return acc + std::norm(pair.second);
                        }));
                } else {
                    return std::sqrt(std::accumulate(values.begin(), values.end(), 0.0,
                        [](double acc, const auto& value) {
                            return acc + std::norm(value);
                        }));
                }

            } else if constexpr (NORM == WhichNorm::ONE) {
                if (!is_compressed()) {
                    return one_norm();
                } else {
                    if constexpr (order == StorageOrder::ROW_MAJOR) {
                        return one_norm_compressed_RowMajor();
                    } else {
                        return one_norm_compressed_ColMajor();
                    }
                }
            } else if constexpr (NORM == WhichNorm::MAX) {
                if (!is_compressed()) {
                    return max_norm();
                } else {
                    if constexpr (order == StorageOrder::ROW_MAJOR) {
                        return max_norm_compressed_RowMajor();
                    } else {
                        return max_norm_compressed_ColMajor();
                    }
                }
            }
        }

        friend std::vector<T> operator*(const Matrix& m, const std::vector<T>& v) {
            if (!m.is_compressed()) {
                return m._matrix_vector_uncompressed(v);
            }
            else{
                if constexpr (order == StorageOrder::ROW_MAJOR) {
                    return m._matrix_vector_row_compressed_RowMajor(v);
                }
                else
                    return m._matrix_vector_row_compressed_ColMajor(v);
            }
        };

        //printing
        friend std::ostream& operator<<(std::ostream& os, const Matrix& m) {
            std::string compr_string = m.is_compressed() ? "Compressed" : "Uncompressed";
            os << "Matrix in ";
            if (m.is_compressed()) {
                os << compr_string << " format" << "with dimensions " << m.rows << "x" << m.cols << ":\n";
                os << "Values: \n";
                for (const auto& v : m.values) {
                    os << v << " ";
                }
                os << "\nRow indices: \n";
                for (const auto& r : m.row_indices) {
                    os << r << " ";
                }
                os << "\nCol indices: \n";
                for (const auto& c : m.col_indices) {
                    os << c << " ";
                }
            } else {
                os << compr_string << " format " << "with dimensions " << m.rows << "x" << m.cols << ":\n";
                for (const auto& [k, v] : m.data) {
                    os << "(" << std::setw(2) << k[0] << ", " << std::setw(2) << k[1] << ")  ->  " << v << "\n";
                }
            }
            return os;
        }

        // extra
        friend Matrix operator*(const Matrix& m1, const Matrix& m2) {
            // ...
        }

        //getters
        std::size_t get_rows() const {
            return rows;
        }
        std::size_t get_cols() const {
            return cols;
        }
   
    private:
        //compression methods
        void compressRowMajor() {
            if (is_compressed())
            {
                return;
            }
            std::size_t num_non_zero = 0;
            row_indices.resize(rows + 1, 0);
            col_indices.resize(data.size());

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
            col_indices.resize(cols + 1, 0);
            row_indices.resize(data.size());

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
    
            for (std::size_t row_idx = 0; row_idx < rows; ++row_idx) {
                for (std::size_t col_idx = row_indices[row_idx]; col_idx < row_indices[row_idx + 1]; ++col_idx) {
                    data[Key{row_idx, col_indices[col_idx]}] = values[col_idx];
                }
            }

            row_indices.clear();
            col_indices.clear();
            values.clear();

            // finalizing
            compressed = false;
        }

        void uncompressColMajor() {
            if (!is_compressed())
            {
                return;
            }
        
            for (std::size_t col_idx = 0; col_idx < cols; ++col_idx) {
                for (std::size_t row_idx = col_indices[col_idx]; row_idx < col_indices[col_idx + 1]; ++row_idx) {
                data[Key{row_indices[row_idx], col_idx}] =values[row_idx];
                }
            }

            row_indices.clear();
            col_indices.clear();
            values.clear();

            // finalizing
            compressed = false;

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
                throw std::runtime_error("Element not found. Cannot change value of an element not stored. One may consider uncompressing the matrix");
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
                throw std::runtime_error("Element not found. Cannot change value of an element not stored. One may consider uncompressing the matrix");
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
                return T();
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
                return T();
            }
        }
        
        //matrix vector multiplication
        std::vector<T> _matrix_vector_uncompressed(std::vector<T> vec) const {
            std::vector<T> out(rows, 0);

            for (const auto& [k, v] : data) {

            out[k[0]] += (vec[k[1]] * v);
            }
            return out;
        }

        std::vector<T> _matrix_vector_row_compressed_RowMajor(std::vector<T> vec) const {

            std::vector<T> out(rows, 0);
            for (std::size_t i = 0; i < rows; ++i) {
                for (std::size_t j = row_indices[i]; j < row_indices[i + 1]; ++j) {
                    out[i] += vec[col_indices[j]] * values[j];
                }
            }

            return out;
        }

        std::vector<T> _matrix_vector_row_compressed_ColMajor(std::vector<T> vec) const {
            std::vector<T> out(rows, 0);
            // iterate through the colums
            for (std::size_t col_idx = 0; col_idx < cols; ++col_idx) {
                for (std::size_t row_idx = col_indices[col_idx]; row_idx < col_indices[col_idx + 1];
                    ++row_idx) {
                out[row_indices[row_idx]] += (vec[col_idx] * values[row_idx]);
                }
            }
            return out;
            }

        //norms
        T one_norm() const {
            std::vector<T> sum_col(cols, 0.0);
            for (const auto& [k, v] : data) {
            sum_col[k[1]] += std::abs(v);
            }
            return *std::max_element(std::begin(sum_col), std::end(sum_col), [](const T& a, const T& b) {
                return std::norm(a) < std::norm(b);
            });
                }
        
        T one_norm_compressed_RowMajor() const {
            std::vector<T> sum_col(cols, 0);
            for (std::size_t col_idx = 0; col_idx < col_indices.size(); ++col_idx) {
                sum_col[col_indices[col_idx]] += std::abs(values[col_idx]);
            }
            return *std::max_element(std::begin(sum_col), std::end(sum_col), [](const T& a, const T& b) {
                return std::norm(a) < std::norm(b);
            });
        }

        T one_norm_compressed_ColMajor() const {
            // std::vector<T> sum_col(cols, 0.0);
            T out = 0;
            for (std::size_t i = 0; i < cols; ++i) {
                T sum = 0;
                for (std::size_t j = col_indices[i]; j < col_indices[i + 1]; ++j) {
                    // sum_col[row_indices[j]] += std::abs(values[j]);
                    sum += std::abs(values[j]);
                }
                out = std::max(out, sum,  [](const T& a, const T& b) {
                return std::norm(a) < std::norm(b);});

            }
            return out;
        }
    
        T max_norm() const {
            std::vector<T> sum_row(rows, 0.0);
            std::accumulate(data.begin(), data.end(), sum_row.begin(), [&](auto acc, const auto& pair) {
                const auto& k = pair.first;
                const auto& v = pair.second;
                acc[k[0]] += std::abs(v);
                return acc;
            });
            return *std::max_element(std::begin(sum_row), std::end(sum_row), [](const T& a, const T& b) {
                return std::norm(a) < std::norm(b);
            });
        }

        T max_norm_compressed_RowMajor() const {
            std::vector<T> sum_row(rows, 0.0);
            for (std::size_t i = 0; i < rows; ++i) {
                for (std::size_t j = row_indices[i]; j < row_indices[i + 1]; ++j) {
                    sum_row[i] += std::abs(values[j]);
                }
            }
            return *std::max_element(std::begin(sum_row), std::end(sum_row), [](const T& a, const T& b) {
                return std::norm(a) < std::norm(b);
            });
        }

        T max_norm_compressed_ColMajor() const {
            std::vector<T> sum_row(rows, 0.0);
            for (std::size_t i = 0; i < cols; ++i) {
                for (std::size_t j = col_indices[i]; j < col_indices[i + 1]; ++j) {
                    sum_row[row_indices[j]] += std::abs(values[j]);
                }
            }
            return *std::max_element(std::begin(sum_row), std::end(sum_row), [](const T& a, const T& b) {
                return std::norm(a) < std::norm(b);
            });
        }
        
    };
}


#endif // MATRIX_HPP
