#include <iostream>
#include <vector>
#include <random>
#include <math.h>
#include <chrono>

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
    int* temp = new int[n * n];
    int sign = sig;
    for (int f = init; f < end; ++f)
    {
        getCofactor(matrix, temp, 0, f, n);
        D += sign * matrix[f] * determinantOfMatrix(temp, n - 1, 0, n - 1, -1);
        sign = -sign;

    }
    delete[] temp;
    return D;
}


int main() {
    int n;
    std::cout << "Enter the size of the matrix:\n";
    std::cin >> n;
    int* matrix = new int[n * n];
    
    generateMatrix(matrix, n * n);
    displayMatrix(matrix, n * n);

    auto start = high_resolution_clock::now();
    long long determinant = determinantOfMatrix(matrix, n, 0, n, 1);
    std::cout << "Determinant: " << determinant << std::endl;

    auto end_time = duration_cast<duration<double>>(high_resolution_clock::now() - start).count();
    std::cout << "Execution time: " << end_time << " seconds" << std::endl;

    return 0;
}


