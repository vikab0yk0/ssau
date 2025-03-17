#include <iostream>
#include <fstream>
#include <unistd.h>
#include <sys/wait.h>
#include <cstring>
#include "matrix.h"


int pipe_in[2], pipe_out[2];


void help();
void sendMatrix(int fd, const Matrix& matrix);
Matrix receiveMatrix(int fd);

void frontend() {
    Matrix matrix;

    std::cin >> matrix;

    std::cout << "Entered Matrix:" << std::endl;
    std::cout << matrix;
    
    sendMatrix(pipe_in[1], matrix);

    Matrix transposed = receiveMatrix(pipe_out[0]);

    std::cout << "Transposed Matrix:" << std::endl;
    std::cout << transposed;

    close(pipe_in[1]);
    close(pipe_out[0]);

    exit(0);
}

void backend() {
    Matrix matrix = receiveMatrix(pipe_in[0]);

    Matrix transposed = matrix.transpose();

    sendMatrix(pipe_out[1], transposed);

    close(pipe_in[0]);
    close(pipe_out[1]);

    exit(0);
}

int main(int argc, char* argv[]) {
    if (argc == 2 && strcmp(argv[1], "--help") == 0) {
        help();
        return 0;
    } else if (argc != 1) {
        std::cerr << "Error: Invalid arguments. Use --help for usage information.\n";
        return 1;
    }

    if (pipe(pipe_in) == -1 || pipe(pipe_out) == -1) {
        std::cerr << "Error: Could not create pipes\n";
        return 1;
    }

    pid_t pid = fork();

    if (pid < 0) {
        std::cerr << "Critical error! Fork has not been created\n";
        return 1;
    } else if (pid > 0) {
        close(pipe_in[0]);
        close(pipe_out[1]);
        frontend();
    } else {
        close(pipe_in[1]);
        close(pipe_out[0]);
        backend();
    }

    close(pipe_in[0]);
    close(pipe_in[1]);
    close(pipe_out[0]);
    close(pipe_out[1]);

    return 0;
}


void help() {
    std::cout << "This program transposes a matrix.\n";
    std::cout << "Usage: ./matrix_transpose [--help]\n";
    std::cout << "Options:\n";
    std::cout << "  --help  Show this help message and exit.\n";
}

void sendMatrix(int fd, const Matrix& matrix) {
    int rows = matrix.getRows();
    int cols = matrix.getCols();

    write(fd, &rows, sizeof(int));
    write(fd, &cols, sizeof(int));
    write(fd, matrix.getData(), rows * cols * sizeof(double));
}

Matrix receiveMatrix(int fd) {
    int rows, cols;
    read(fd, &rows, sizeof(int));
    read(fd, &cols, sizeof(int));

    Matrix matrix(rows, cols);
    read(fd, matrix.getData(), rows * cols * sizeof(double));

    return matrix;
}