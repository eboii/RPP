#include <iostream>
#include <chrono>
#include <vector>
#include <algorithm>
#include <cmath>
#include <mpi.h>

using namespace std;
using namespace std::chrono;

// Функція для отримання розряду числа
long getDigit(long num, int digitPlace) {
    return (num / static_cast<long>(pow(10, digitPlace))) % 10;
}

// Функція для радіксного сортування
void radixSort(vector<long>& arr) {
    int maxDigit = 0;
    for (long num : arr) {
        int numDigits = static_cast<int>(log10(num)) + 1;
        if (numDigits > maxDigit)
            maxDigit = numDigits;
    }

    // Сортуємо за кожним розрядом
    for (int digitPlace = 0; digitPlace < maxDigit; digitPlace++) {
        vector<vector<long>> buckets(10);

        for (size_t i = 0; i < arr.size(); i++) {
            long digit = getDigit(arr[i], digitPlace);
            buckets[digit].push_back(arr[i]);
        }

        // Збираємо дані з бакетів
        size_t index = 0;
        for (vector<long>& bucket : buckets) {
            for (long num : bucket) {
                arr[index++] = num;
            }
        }
    }
}

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);

    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    // Генеруємо випадковий масив для сортування
    vector<long> arr(1000000);
    srand(time(nullptr) + world_rank * 100); // Забезпечуємо різні початкові значення для кожного процесу
    for (long& num : arr) {
        num = rand() % 10000;
        num *= rand() % 1000;
    }

    // Поділ масиву між процесами
    int local_size = arr.size() / world_size;
    vector<long> local_arr(local_size);
    MPI_Scatter(arr.data(), local_size, MPI_LONG, local_arr.data(), local_size, MPI_LONG, 0, MPI_COMM_WORLD);

    auto start = high_resolution_clock::now();
    radixSort(local_arr);

    // Збір відсортованих даних на процесі 0
    vector<long> sorted_arr(arr.size());
    MPI_Gather(local_arr.data(), local_size, MPI_LONG, sorted_arr.data(), local_size, MPI_LONG, 0, MPI_COMM_WORLD);

    auto end = high_resolution_clock::now();

    // Перевіряємо, чи масив сортується правильно
    if (world_rank == 0) {
        cout << "Sorted array:" << endl;
        for (long num : sorted_arr) {
            cout << num << " ";
        }
        cout << endl;

        auto duration = duration_cast<milliseconds>(end - start);
        cout << "Execution time: " << duration.count() << " milliseconds" << endl;
    }

    MPI_Finalize();
    return 0;
}
