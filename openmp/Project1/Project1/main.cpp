#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include <chrono>
#include <random>
#include <algorithm>
#include <omp.h> 

using namespace std;
using namespace std::chrono;

void shellSort(vector<double>& arr) {
    int n = arr.size();
    // Parallelize the outer loop of the Shell sort
    for (int gap = n / 2; gap > 0; gap /= 2) {
#pragma omp parallel for default(none) shared(arr, gap, n)
        for (int i = gap; i < n; i++) {
            double temp = arr[i];
            int j;
            for (j = i; j >= gap && arr[j - gap] > temp; j -= gap) {
                arr[j] = arr[j - gap];
            }
            arr[j] = temp;
        }
    }
}

int main(int argc, char* argv[]) {
    int n = (argc > 1) ? stoi(argv[1]) : 100000;
    vector<double> data(n);

    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<> dis(0, 1);

    // Initialize data
    for (int i = 0; i < n; ++i) {
        data[i] = dis(gen);
    }

    cout << "Initial data:" << endl;
    for (const auto& val : data) {
        //cout << val << " ";
    }
    cout << endl;

    auto start = high_resolution_clock::now();

    omp_set_num_threads(5); // adjust this based on your CPU cores
    shellSort(data);

    auto end = high_resolution_clock::now();

    cout << "Sorted data:" << endl;
    for (const auto& val : data) {
        //cout << val << " ";
    }
    cout << endl;

    auto duration = duration_cast<milliseconds>(end - start);
    cout << "Execution time: " << duration.count() << " milliseconds" << endl;

    return 0;
}
