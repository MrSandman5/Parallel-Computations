#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <cmath>
#include <cstdlib>
#include <chrono>
#include <random>

#define MASTER 0
#define FROM_MASTER 1
#define FROM_WORKER 2

using namespace std::chrono;

void generateMatrix(int* matrix, int n) {
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_int_distribution<int> dist(-10, 10);
    for (int i = 0; i < n; ++i) {
        matrix[i] = dist(mt);
    }
}

void displayMatrix(int* matrix, int n)
{
    std::cout << "Matrix:" << std::endl;
    for (int i = 0; i < n; ++i) {
        std::cout << matrix[i] << " ";

        if ((i + 1) % (int)sqrt(n) == 0) {
            std::cout << "\n";
        }
    }
    std::cout << "\n";
}

void getCofactor(int* matrix, int* temp, int p, int q, int n)
{
    int i = 0, j = 0;
    for (int row = 0; row < n; ++row)
    {
        for (int col = 0; col < n; ++col)
        {
            if (row != p && col != q)
            {
                temp[i * (n - 1) + j++] = matrix[row * n + col];
                if (j == n - 1)
                {
                    j = 0;
                    i++;
                }
            }
        }
    }
}

long long determinantOfMatrix(int* matrix, int n, int init, int end, int sig)
{
    long long D = 0;
    if (n == 1) {
        return matrix[0];
    }
    int* temp = new int [n * n];
    int sign = sig;
    for (int f = init; f < end; ++f)
    {
        getCofactor(matrix, temp, 0, f, n);
        D += sign * matrix[f] * determinantOfMatrix(temp, n - 1, 0, n - 1, 1);
        sign = -sign;

    }
    delete[] temp;
    return D;
}

int main(int argc, char* argv[]) {
    int numtasks,
        taskid,
        numworkers,
        rc = 0;

    int n = atoi(argv[1]);
    int* matrix = new int[n * n];
    long long det;
    long long determinant = 0;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &taskid);
    MPI_Comm_size(MPI_COMM_WORLD, &numtasks);

    if (numtasks < 2) {
        printf("Need to have at least 2 processes. Aborting.\n");
        MPI_Abort(MPI_COMM_WORLD, rc);
        exit(1);
    }
    numworkers = numtasks - 1;
    int* res = new int[numworkers];

    if (taskid == MASTER) {
        printf("mpi_mm initialization of %d processes.\n\n", numtasks);

        generateMatrix(matrix, n * n);
        displayMatrix(matrix, n * n);

        auto start = high_resolution_clock::now();

        for (int i = 1; i < numtasks; ++i) {
            MPI_Send(matrix, n * n, MPI_INTEGER, i, 0, MPI_COMM_WORLD);
        }

        for (int i = 1; i < numtasks; ++i) {
            MPI_Recv(&res[i - 1], 1, MPI_INTEGER, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }

        for (int i = 0; i < numworkers; ++i) {
            determinant += res[i];
        }

        std::cout << "Determinant: " << determinant << std::endl;
        auto end_time = duration_cast<duration<double>>(high_resolution_clock::now() - start).count();
        std::cout << "Execution time: " << end_time << " seconds" << std::endl;
    }

    else if (taskid > MASTER) {

        MPI_Recv(matrix, n * n, MPI_INTEGER, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        int init = (n / numworkers * (taskid - 1));
        int end = (n / numworkers * taskid);
        int sig = init % 2 == 0 ? 1 : -1;

        if (taskid == numworkers) {
            end = n;
        }

        det = determinantOfMatrix(matrix, n, init, end, sig);

        MPI_Send(&det, 1, MPI_INTEGER, 0, 0, MPI_COMM_WORLD);
    }

    delete[] res;
    delete[] matrix;
    MPI_Finalize();
}