#include <array>
#include <functional>

namespace algebra {
    enum StorageOrder {
        ROW_MAJOR,
        COL_MAJOR
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

    template <typename T, StorageOrder order>
    using Compare = std::conditional_t<order == ROW_MAJOR, std::less<Key>, ColumnMajorCompare>;
    //by default we have the normal std::less<Key> for ROW_MAJOR


}