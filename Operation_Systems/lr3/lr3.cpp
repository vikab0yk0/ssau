#include <iostream>
#include <fstream>
#include <unistd.h>
#include <sys/wait.h>
#include <cstring>
#include "matrix.h"


void parseArguments(int argc, char* argv[], char*& inputFile, char*& outputFile);
void sendMatrix(int fd, const Matrix& matrix);
Matrix receiveMatrix(int fd);
Matrix readMatrix(const char* inputFile);
void printMatrix(const Matrix& matrix, const char* outputFile, const char* message);
void help();


void frontend(const char* inputFile, const char* outputFile, int write_fd, int read_fd) {
    Matrix matrix = readMatrix(inputFile);
    printMatrix(matrix, outputFile, "Entered Matrix:");

    sendMatrix(write_fd, matrix);

    Matrix transposed = receiveMatrix(read_fd);

    printMatrix(transposed, outputFile, "Transposed Matrix:");

    close(write_fd);
    close(read_fd);
    exit(0);
}

void backend(int read_fd, int write_fd) {
    Matrix matrix = receiveMatrix(read_fd);

    Matrix transposed = matrix.transpose();
    sendMatrix(write_fd, transposed);

    close(read_fd);
    close(write_fd);
    exit(0);
}


int main(int argc, char* argv[]) {
    char* inputFile = nullptr;
    char* outputFile = nullptr;

    parseArguments(argc, argv, inputFile, outputFile);

    int pipe_in[2], pipe_out[2];

    // Создание каналов
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

        frontend(inputFile, outputFile, pipe_in[1], pipe_out[0]);
    }
    else {
        close(pipe_in[1]);
        close(pipe_out[0]);

        backend(pipe_in[0], pipe_out[1]);
    }

    close(pipe_in[0]);
    close(pipe_in[1]);
    close(pipe_out[0]);
    close(pipe_out[1]);

    if (inputFile) delete[] inputFile;
    if (outputFile) delete[] outputFile;

    return 0;
}


void parseArguments(int argc, char* argv[], char*& inputFile, char*& outputFile) {
    for (int i = 1; i < argc; ++i) {
        if (!strcmp(argv[i], "-i") && i + 1 < argc) {
            inputFile = new char[strlen(argv[i + 1]) + 1];
            strcpy(inputFile, argv[i + 1]);
            i++;
        }
        else if (!strcmp(argv[i], "-o") && i + 1 < argc) {
            outputFile = new char[strlen(argv[i + 1]) + 1];
            strcpy(outputFile, argv[i + 1]);
            i++;
        }
        else if (!strcmp(argv[i], "--help")) {
            help();
            exit(0);
        }
        else {
            std::cerr << "Error: Invalid argument " << argv[i] << "\n";
            help();
            exit(1);
        }
    }
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


Matrix readMatrix(const char* inputFile) {
    Matrix matrix;

    if (inputFile) {
        std::ifstream fin(inputFile);
        if (!fin.is_open()) {
            std::cerr << "Error: Could not open input file " << inputFile << std::endl;
            exit(1);
        }
        fin >> matrix;
        fin.close();
    }
    else std::cin >> matrix;

    return matrix;
}

void printMatrix(const Matrix& matrix, const char* outputFile, const char* message) {
    if (outputFile) {
        std::ofstream fout(outputFile);
        if (!fout.is_open()) {
            std::cerr << "Error: Could not open output file " << outputFile << std::endl;
            exit(1);
        }
        fout << message << std::endl;
        fout << matrix;
        fout.close();
    }
    else std::cout << message << std::endl << matrix;
}


void help() {
    std::cout << "This program transposes a matrix.\n"
              << "Usage: ./lr3 [OPTIONS]\n"
              << "Options:\n"
              << "  -i <filename>  Input file (optional)\n"
              << "  -o <filename>  Output file (optional)\n"
              << "  --help         Show this help message and exit\n";
}