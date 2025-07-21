#include <iostream>
#include <vector>
#include <cassert>
#include <cmath>
#include "geometry.h"

// A small tolerance for comparing floating-point numbers.
const float EPSILON = 1e-6;

// Helper function to check if two matrices are approximately equal.
bool areMatricesEqual(Matrix& a, Matrix& b) {
    if (a.nrows() != b.nrows() || a.ncols() != b.ncols()) {
        return false;
    }
    for (int i = 0; i < a.nrows(); ++i) {
        for (int j = 0; j < a.ncols(); ++j) {
            if (std::abs(a[i][j] - b[i][j]) > EPSILON) {
                return false;
            }
        }
    }
    return true;
}

// Test case for the Matrix constructor and basic accessors.
void test_constructor_and_accessors() {
    std::cout << "Running test_constructor_and_accessors..." << std::endl;
    Matrix A(3, 4);
    assert(A.nrows() == 3);
    assert(A.ncols() == 4);

    // Check if all elements are initialized to 0.
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 4; ++j) {
            assert(A[i][j] == 0.f);
        }
    }
    std::cout << "PASSED" << std::endl;
}

// Test case for creating an identity matrix.
void test_identity() {
    std::cout << "Running test_identity..." << std::endl;
    Matrix I = Matrix::identity(4);
    assert(I.nrows() == 4);
    assert(I.ncols() == 4);

    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            if (i == j) {
                assert(I[i][j] == 1.f);
            } else {
                assert(I[i][j] == 0.f);
            }
        }
    }
    std::cout << "PASSED" << std::endl;
}

// Test case for matrix multiplication.
void test_multiplication() {
    std::cout << "Running test_multiplication..." << std::endl;
    Matrix A(2, 2);
    A[0][0] = 1; A[0][1] = 2;
    A[1][0] = 3; A[1][1] = 4;

    Matrix B(2, 2);
    B[0][0] = 5; B[0][1] = 6;
    B[1][0] = 7; B[1][1] = 8;

    Matrix C = A * B;
    assert(C.nrows() == 2);
    assert(C.ncols() == 2);

    Matrix expected(2, 2);
    expected[0][0] = 19; expected[0][1] = 22;
    expected[1][0] = 43; expected[1][1] = 50;

    assert(areMatricesEqual(C, expected));
    std::cout << "PASSED" << std::endl;
}

// Test case for matrix transpose.
void test_transpose() {
    std::cout << "Running test_transpose..." << std::endl;
    Matrix A(2, 3);
    A[0][0] = 1; A[0][1] = 2; A[0][2] = 3;
    A[1][0] = 4; A[1][1] = 5; A[1][2] = 6;

    Matrix T = A.transpose();
    assert(T.nrows() == 3);
    assert(T.ncols() == 2);

    Matrix expected(3, 2);
    expected[0][0] = 1; expected[0][1] = 4;
    expected[1][0] = 2; expected[1][1] = 5;
    expected[2][0] = 3; expected[2][1] = 6;

    assert(areMatricesEqual(T, expected));
    std::cout << "PASSED" << std::endl;
}

// Test case for matrix inversion.
void test_inverse() {
    std::cout << "Running test_inverse..." << std::endl;
    Matrix A(2, 2);
    A[0][0] = 1; A[0][1] = 2;
    A[1][0] = 3; A[1][1] = 4;

    Matrix A_inv = A.inverse();
    Matrix I = A * A_inv;

    Matrix identity = Matrix::identity(2);
    
    // Note: The inverse logic in geometry.cpp seems to have issues.
    // This test will likely fail until the inverse function is corrected.
    // A correct inverse for A would be:
    // [[-2, 1], [1.5, -0.5]]
    assert(areMatricesEqual(I, identity));
    std::cout << "PASSED" << std::endl;
}


int main() {
    try {
        test_constructor_and_accessors();
        test_identity();
        test_multiplication();
        test_transpose();
        // The inverse test is commented out as it is expected to fail with the current implementation.
        // You can uncomment it after debugging the Matrix::inverse() method.
        test_inverse(); 
        

    } catch (const std::exception& e) {
        std::cerr << "A test failed with exception: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "A test failed with an unknown exception." << std::endl;
        return 1;
    }

    std::cout << "\nAll enabled tests completed." << std::endl;
    return 0;
}
