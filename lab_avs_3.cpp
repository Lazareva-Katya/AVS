#include <iostream>
#include <chrono>
#include <omp.h>

using namespace std;

int** fill_matrix(int n, int** matrix)
{
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			matrix[i][j] = rand() % 35;
		}
	}
	return matrix;
}

int* fill_v(int n, int* v, int** matrix)
{
	for (int i = 0; i < n; i++) {
		v[i] = rand() % 35;
		matrix[i] = new int[n];
	}
	return v;
}

void first(int n, int* v, int** matrix)
{
	int res = 0;
	auto start = chrono::high_resolution_clock::now();
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			res += v[j] * matrix[i][j];
		}
	}
	auto finish = chrono::high_resolution_clock::now();
	double first_time = chrono::duration_cast<chrono::nanoseconds>(finish - start).count() * 1e-9;
	cout << "Result with one thread:" << res;
	cout << endl << "Time: " << first_time << "sec." << endl;
}

void second(int n, int* v, int** matrix)
{
	int res = 0;
	auto start = chrono::high_resolution_clock::now();
	#pragma omp parallel for reduction(+:res) schedule(static)
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			res += v[j] * matrix[i][j];
		}
	}
	auto finish = chrono::high_resolution_clock::now();
	double first_time = chrono::duration_cast<chrono::nanoseconds>(finish - start).count() * 1e-9;
	cout << "Parallel result:" << res;
	cout << endl << "Time: " << first_time << "sec." << endl;
}


int main()
{
	int n = 20000;
	int** matrix = new int* [n];
	int* v = new int[n];
	v = fill_v(n, v, matrix);
	matrix = fill_matrix(n, matrix);
	first(n, v, matrix);
	second(n, v, matrix);
	return 0;
}