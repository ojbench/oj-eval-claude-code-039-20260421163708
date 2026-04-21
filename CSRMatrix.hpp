#ifndef CSR_MATRIX_HPP
#define CSR_MATRIX_HPP

#include <vector>
#include <exception>
#include <algorithm>
#include <stdexcept>

namespace sjtu {

class size_mismatch : public std::exception {
public:
    const char *what() const noexcept override {
        return "Size mismatch";
    }
};

class invalid_index : public std::exception {
public:
    const char *what() const noexcept override {
        return "Index out of range";
    }
};

template <typename T>
class CSRMatrix {

private:
    size_t rows_;
    size_t cols_;
    std::vector<size_t> indptr_;
    std::vector<size_t> indices_;
    std::vector<T> data_;

public:
    // Assignment operators are deleted
    CSRMatrix &operator=(const CSRMatrix &other) = delete;
    CSRMatrix &operator=(CSRMatrix &&other) = delete;

    // Constructor for empty matrix with dimensions
    CSRMatrix(size_t n, size_t m) : rows_(n), cols_(m), indptr_(n + 1, 0) {}

    // Constructor with pre-built CSR components
    CSRMatrix(size_t n, size_t m, size_t count,
        const std::vector<size_t> &indptr,
        const std::vector<size_t> &indices,
        const std::vector<T> &data) : rows_(n), cols_(m), indptr_(indptr), indices_(indices), data_(data) {
        if (indptr_.size() != n + 1) throw size_mismatch();
        if (indices_.size() != count) throw size_mismatch();
        if (data_.size() != count) throw size_mismatch();
    }

    // Copy constructor
    CSRMatrix(const CSRMatrix &other) = default;

    // Move constructor
    CSRMatrix(CSRMatrix &&other) = default;

    // Constructor from dense matrix format (given as vector of vectors)
    CSRMatrix(size_t n, size_t m, const std::vector<std::vector<T>> &dense_data) : rows_(n), cols_(m) {
        indptr_.push_back(0);
        for (size_t i = 0; i < n; ++i) {
            for (size_t j = 0; j < m; ++j) {
                if (!(dense_data[i][j] == T())) {
                    indices_.push_back(j);
                    data_.push_back(dense_data[i][j]);
                }
            }
            indptr_.push_back(indices_.size());
        }
    }

    // Destructor
    ~CSRMatrix() = default;

    // Get dimensions and non-zero count
    size_t getRowSize() const { return rows_; }
    size_t getColSize() const { return cols_; }
    size_t getNonZeroCount() const { return data_.size(); }

    // Element access
    T get(size_t i, size_t j) const {
        if (i >= rows_ || j >= cols_) throw invalid_index();
        size_t start = indptr_[i];
        size_t end = indptr_[i + 1];
        auto it = std::lower_bound(indices_.begin() + start, indices_.begin() + end, j);
        if (it != indices_.begin() + end && *it == j) {
            return data_[start + std::distance(indices_.begin() + start, it)];
        }
        return T();
    }

    void set(size_t i, size_t j, const T &value) {
        if (i >= rows_ || j >= cols_) throw invalid_index();
        size_t start = indptr_[i];
        size_t end = indptr_[i + 1];
        auto it = std::lower_bound(indices_.begin() + start, indices_.begin() + end, j);
        size_t pos = std::distance(indices_.begin(), it);

        if (it != indices_.begin() + end && *it == j) {
            data_[pos] = value;
        } else {
            indices_.insert(it, j);
            data_.insert(data_.begin() + pos, value);
            for (size_t k = i + 1; k <= rows_; ++k) {
                indptr_[k]++;
            }
        }
    }

    // Access CSR components
    const std::vector<size_t> &getIndptr() const { return indptr_; }
    const std::vector<size_t> &getIndices() const { return indices_; }
    const std::vector<T> &getData() const { return data_; }

    // Convert to dense matrix format
    std::vector<std::vector<T>> getMatrix() const {
        std::vector<std::vector<T>> res(rows_, std::vector<T>(cols_, T()));
        for (size_t i = 0; i < rows_; ++i) {
            for (size_t k = indptr_[i]; k < indptr_[i + 1]; ++k) {
                res[i][indices_[k]] = data_[k];
            }
        }
        return res;
    }

    // Matrix-vector multiplication
    std::vector<T> operator*(const std::vector<T> &vec) const {
        if (vec.size() != cols_) throw size_mismatch();
        std::vector<T> res(rows_, T());
        for (size_t i = 0; i < rows_; ++i) {
            for (size_t k = indptr_[i]; k < indptr_[i + 1]; ++k) {
                res[i] += data_[k] * vec[indices_[k]];
            }
        }
        return res;
    }

    // Row slicing
    CSRMatrix getRowSlice(size_t l, size_t r) const {
        if (l >= r || r > rows_) throw invalid_index();
        size_t new_n = r - l;
        CSRMatrix res(new_n, cols_);
        res.indptr_.clear();
        res.indptr_.push_back(0);
        size_t offset = indptr_[l];
        for (size_t i = l; i < r; ++i) {
            for (size_t k = indptr_[i]; k < indptr_[i + 1]; ++k) {
                res.indices_.push_back(indices_[k]);
                res.data_.push_back(data_[k]);
            }
            res.indptr_.push_back(res.indices_.size());
        }
        return res;
    }
};

}

#endif // CSR_MATRIX_HPP
