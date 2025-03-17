#include "matrix.h"
#include <cstdio>


int read_int(const char* msg) {
    std::cout << msg;
    int input;
    while (!(std::cin >> input) || std::cin.peek() != '\n') {
        std::cin.clear();
        while (std::cin.get() != '\n');
        std::cout << "Input error. Retry input: ";
    }
    return input;
}

double read_double(const char* msg) {
    std::cout << msg;
    double input;
    while (!(std::cin >> input) || std::cin.peek() != '\n') {
        std::cin.clear();
        while (std::cin.get() != '\n');
        std::cout << "Input error. Retry input: ";
    }
    return input;
}


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
    mat.rows = read_int("Enter the number of rows: ");
    mat.cols = read_int("Enter the number of columns: ");

    mat.data = new double[mat.rows*mat.cols];

    std::cout << "Enter the matrix elements: " << std::endl;
    for (int i = 0; i < mat.rows; ++i) {
        for (int j = 0; j < mat.cols; ++j) {
            int size = snprintf(nullptr, 0, "Element [%d][%d]: ", i, j) + 1;

            char* elem = new char[size];
            snprintf(elem, size, "Element [%d][%d]: ", i, j);

            mat.data[i * mat.cols + j] = read_double(elem);

            delete[] elem;
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
