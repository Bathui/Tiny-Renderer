#include <vector>
#include <cassert>
#include "geometry.h"

template <> template<>
Vec3<int>::Vec3<>(const Vec3<float>& v): x(int(v.x + 0.5)), y(int(v.y + 0.5)), z(int(v.z + 0.5)) {}

template<>template<>
Vec3<float>::Vec3<>(const Vec3<int>& v):x(v.x), y(v.y), z(v.z) {}

Matrix::Matrix(int r, int c): rows(r), cols(c), m(std::vector<std::vector<float>>(r, std::vector<float>(c, 0.f))){}

std::vector<float>& Matrix::operator[](const int i){
    assert(i >= 0 && i < rows);
    return m[i];
}

Matrix Matrix::identity(int dimensions){
    Matrix m (dimensions, dimensions);
    for (int i =0; i < dimensions; i++) {
        for (int j = 0; j < dimensions; j++) {
            m[i][j] = i==j ? 1.f : 0.f;
        }
    }
    return m;
}

Matrix Matrix::operator* (const Matrix& a){
    assert(cols == a.rows);
    Matrix res(rows, a.cols);

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < a.cols; j++) {
            res.m[i][j] = 0.f;
            for (int k = 0; k < cols; k++) {
                res.m[i][j] += m[i][k] * a.m[k][j];
            }
        }
    }

    return res;
} 

Matrix Matrix::transpose(){
    Matrix res(cols, rows);

    for (int i = 0; i < cols; i++) {
        for (int j = 0; j < rows; j++) {
            res[i][j] = m[j][i];
        }
    }

    return res;
}

Matrix Matrix::inverse(){
    assert(rows == cols);

    Matrix result(rows, cols * 2);
    
    // get augmented matrix
    // Following is to copy the orginal matrix
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            result[i][j] = m[i][j];
        }
    }
    // Following is to augment the identity matrix
    for (int i = 0; i < rows; i++) {
        result[i][rows + i] = 1;
    }

    // forward elimination
    for (int i = 0; i < rows-1; i++) {
        for (int j = 0; j < result.cols; j++) {
            result[i][j] /= result[i][i];
        }
        for (int k = i+1; k < rows; k++) {
            float coefficient = result[k][i];
            for (int j = 0; j < result.cols; j++) {
                result[k][j] -= coefficient * result[i][j];
            }
        }
    }

    //Normalize the last row
    float factor = result[rows-1][cols-1];
    for (int j = cols-1; j < result.cols; j++) {
        result[rows-1][j] /= factor;
    }

    //Backward elimination
    for (int i = rows-1; i>0; i--) {
        for (int k = i - 1; k >= 0; k--) {
            float coefficient = result[k][i];
            for (int j = 0; j < result.cols; j++) {
                result[k][j] -= coefficient * result[i][j];
            }
        }
    }

    //Truncate to get the inverse matrix
    Matrix truncate(rows, cols);
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            truncate[i][j] = result[i][j+cols];
        }
    }

    return truncate;
}

std::ostream& operator<<(std::ostream& s, Matrix& m){
    for (int i = 0; i < m.rows; i++) {
        for (int j = 0; j < m.cols; j++) {
            s<<m[i][j];
            if (j == m.cols-1)
                s<<'\t';
        }
        s<<'\n';
    }
    return s;
}