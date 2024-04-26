#ifndef UTILS_HPP
#define UTILS_HPP
#include <array>
#include <functional>
#include <type_traits>
#include <random>


namespace algebra {

    template <typename T>
    concept Numeric = std::is_arithmetic_v<T> || std::is_same_v<T, std::complex<float>> || std::is_same_v<T, std::complex<double>>;

    enum StorageOrder {
        ROW_MAJOR,
        COL_MAJOR
    };

    enum WhichNorm {
        FROBENIUS,
        ONE,
        MAX
    };

    using Key = std::array<std::size_t, 2>;

    struct ColumnMajorCompare {
        bool operator()(const Key& a, const Key& b) const {
            // Compare columns first, then rows
            if (a[1] < b[1]) return true;
            if (a[1] > b[1]) return false;
            return a[0] < b[0];
        }
    };

    template <Numeric T, StorageOrder order>
    using Compare = std::conditional_t<order == ROW_MAJOR, std::less<Key>, ColumnMajorCompare>;
    //by default we have the normal std::less<Key> for ROW_MAJOR
    
        
    template <Numeric T>
    std::vector<T> vector_generator(std::size_t size,
                                    double lower_bound = -10,
                                    double upper_bound = 10) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<double> dis(lower_bound, upper_bound);

        std::vector<T> random_vector(size);
        if constexpr (std::is_same_v<T, std::complex<float>> || std::is_same_v<T, std::complex<double>>) {
            for (std::size_t i = 0; i < size; ++i) {
                double real_part = dis(gen); 
                double imag_part = dis(gen);
                random_vector[i] = T(real_part, imag_part);
            }
        } else {
            for (int i = 0; i < size; ++i) {
                random_vector[i] = dis(gen);
            }
        }
        return random_vector;
    }


}
#endif