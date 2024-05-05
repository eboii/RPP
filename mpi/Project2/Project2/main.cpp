#include <iostream>
#include <vector>
#include <cmath>
#include <mpi.h>
#include <string>
#include <chrono>
#include <random>
#include <algorithm>
using namespace std;
using namespace std::chrono;
void shellSort(vector<double>& arr) {
	int n = arr.size();
	for (int gap = n / 2; gap > 0; gap /= 2) {
		for (int i = gap; i < n; i += 1) {
			double temp = arr[i];
			int j;
			for (j = i; j >= gap && arr[j - gap] > temp; j -= gap)
				arr[j] = arr[j - gap];
			arr[j] = temp;
		}
	}
}
int main(int argc, char* argv[]) {
	MPI_Init(&argc, &argv);
	int size, rank;
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	int n = (argc > 1) ? stoi(argv[1]) : 100000;
	int local_n = n / size;
	vector<double> data(n), local_data(local_n);
	if (rank == 0) {
		random_device rd;
		mt19937 gen(rd());
		uniform_real_distribution<> dis(0, 1);
		for (int i = 0; i < n; ++i) {
			data[i] = dis(gen);
		}
		cout << "Initial data:" << endl;
		for (const auto& val : data) {
			cout << val << " ";
		}
	}
	MPI_Scatter(data.data(), local_n, MPI_DOUBLE, local_data.data(), local_n, MPI_DOUBLE,
		0, MPI_COMM_WORLD);
	auto start = high_resolution_clock::now();
	shellSort(local_data);
	MPI_Gather(local_data.data(), local_n, MPI_DOUBLE, data.data(), local_n, MPI_DOUBLE,
		0, MPI_COMM_WORLD);
	auto end = high_resolution_clock::now();
	if (rank == 0) {
		shellSort(data);
		cout << endl << "Sorted data:" << endl;
		for (const auto& val : data)
			cout << val << " ";
		cout << endl;
		auto duration = duration_cast<milliseconds>(end - start);
		cout << "Execution time: " << duration.count() << " milliseconds" << endl;
	}
	MPI_Finalize();
	return 0;
}
