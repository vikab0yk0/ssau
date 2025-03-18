#ifndef MATRIX_H
#define MATRIX_H

#include <iostream>
#include <fstream>


class Matrix {
private:
    int rows, cols;
    double* data;

public:
    Matrix(int r = 0, int c = 0);
    Matrix(const Matrix& other);
    ~Matrix();

    Matrix& operator=(const Matrix& other);

    Matrix transpose() const;

    friend std::istream& operator>>(std::istream& in, Matrix& mat);
    friend std::ostream& operator<<(std::ostream& out, const Matrix& mat);

    int getRows() const { return rows; }
    int getCols() const { return cols; }
    double* getData() const { return data; }
};

#endif // MATRIX_H