#include "matrix.h"
#include <cstdio>

Matrix::Matrix(int r, int c) : rows(r), cols(c) {
    if (rows > 0 && cols > 0)
        data = new double[rows*cols];
    else
        data = nullptr;
}

Matrix::Matrix(const Matrix& other) : rows(other.rows), cols(other.cols) {
    if (rows > 0 && cols > 0) {
        data = new double[rows*cols];
        for (int i = 0; i < rows*cols; ++i)
            data[i] = other.data[i];
    }
    else
        data = nullptr;
}

Matrix::~Matrix() {
    if (data) {
        delete[] data;
        data = nullptr;
    }
}


Matrix& Matrix::operator=(const Matrix& other) {
    if (this == &other) {
        return *this;
    }

    if (data)
        delete[] data;

    rows = other.rows;
    cols = other.cols;

    if (rows > 0 && cols > 0) {
        data = new double[rows * cols];
        for (int i = 0; i < rows * cols; ++i)
            data[i] = other.data[i];
    }
    else data = nullptr;

    return *this;
}


Matrix Matrix::transpose() const {  // новый_индекс = (старый_индекс % N) * M + (старый_индекс / N)
    Matrix transposed(cols, rows);

    for (int i = 0; i < cols*rows; ++i)
        transposed.data[(i % cols) * rows + (i / cols)] = data[i];

    return transposed;
}


std::istream& operator>> (std::istream& in, Matrix& mat) {
    if (&in == &std::cin) std::cout << "Enter the number of rows: ";
    in >> mat.rows;

    if (&in == &std::cin) std::cout << "Enter the number of columns: ";
    in >> mat.cols;

    mat.data = new double[mat.rows * mat.cols];

    if (&in == &std::cin) std::cout << "Enter the matrix elements:\n";

    for (int i = 0; i < mat.rows; ++i) {
        for (int j = 0; j < mat.cols; ++j) {
            if (&in == &std::cin) std::cout << "Element [" << i << "][" << j << "]: ";
            in >> mat.data[i * mat.cols + j];
        }
    }
    return in;
}

std::ostream& operator<<(std::ostream& out, const Matrix& mat) {
    for (int i = 0; i < mat.rows; ++i) {
        for (int j = 0; j < mat.cols; ++j)
            out << mat.data[i * mat.cols + j] << "\t";
        out << std::endl;
    }
    return out;
}
