#include "CSRMatrix.hpp"
#include <iostream>
#include <vector>
#include <cassert>

using namespace sjtu;

void test_basic() {
    CSRMatrix<int> mat(3, 3);
    assert(mat.getRowSize() == 3);
    assert(mat.getColSize() == 3);
    assert(mat.getNonZeroCount() == 0);
    assert(mat.get(0, 0) == 0);

    mat.set(0, 0, 1);
    mat.set(1, 1, 2);
    mat.set(2, 2, 3);
    assert(mat.getNonZeroCount() == 3);
    assert(mat.get(0, 0) == 1);
    assert(mat.get(1, 1) == 2);
    assert(mat.get(2, 2) == 3);
    assert(mat.get(0, 1) == 0);

    std::vector<int> vec = {1, 2, 3};
    std::vector<int> res = mat * vec;
    assert(res[0] == 1);
    assert(res[1] == 4);
    assert(res[2] == 9);

    auto dense = mat.getMatrix();
    assert(dense[0][0] == 1);
    assert(dense[1][1] == 2);
    assert(dense[2][2] == 3);

    CSRMatrix<int> slice = mat.getRowSlice(1, 3);
    assert(slice.getRowSize() == 2);
    assert(slice.getNonZeroCount() == 2);
    assert(slice.get(0, 1) == 2);
    assert(slice.get(1, 2) == 3);

    std::cout << "test_basic passed!" << std::endl;
}

void test_dense_constructor() {
    std::vector<std::vector<int>> dense = {
        {1, 0, 0},
        {0, 2, 0},
        {0, 0, 3}
    };
    CSRMatrix<int> mat(3, 3, dense);
    assert(mat.getNonZeroCount() == 3);
    assert(mat.get(0, 0) == 1);
    assert(mat.get(1, 1) == 2);
    assert(mat.get(2, 2) == 3);
    std::cout << "test_dense_constructor passed!" << std::endl;
}

void test_exceptions() {
    CSRMatrix<int> mat(3, 3);
    try {
        mat.get(3, 0);
        assert(false);
    } catch (const invalid_index& e) {
        // expected
    }
    try {
        mat.set(0, 3, 1);
        assert(false);
    } catch (const invalid_index& e) {
        // expected
    }
    std::vector<int> vec = {1, 2};
    try {
        mat * vec;
        assert(false);
    } catch (const size_mismatch& e) {
        // expected
    }
    std::cout << "test_exceptions passed!" << std::endl;
}

int main() {
    test_basic();
    test_dense_constructor();
    test_exceptions();
    return 0;
}
