#include <iostream>
#include <omp.h>
#include <chrono>
#include <vector>
#include <algorithm>
#include <cmath>

using namespace std;
using namespace std::chrono;

// отримання розряду числа
long getDigit(long num, int digitPlace) {
    return (num / static_cast<long>(pow(10, digitPlace))) % 10;
}

//сортування розрядами
void radixSort(vector<long>& arr, int num_threads) {
    int maxDigit = 0;
    for (long num : arr) {
        int numDigits = static_cast<int>(log10(num)) + 1;
        if (numDigits > maxDigit)
            maxDigit = numDigits;
    }

    // підготовка масиву бакетів для кожного потоку
    vector<vector<vector<long>>> thread_buckets(num_threads, vector<vector<long>>(10));

    // паралельно сортуємо за кожним розрядом
    for (int digitPlace = 0; digitPlace < maxDigit; digitPlace++) {
#pragma omp parallel num_threads(num_threads)
        {
            int thread_id = omp_get_thread_num();
            vector<vector<long>>& buckets = thread_buckets[thread_id];

#pragma omp for
            for (size_t i = 0; i < arr.size(); i++) {
                long digit = getDigit(arr[i], digitPlace);
                buckets[digit].push_back(arr[i]);
            }
        }

        // збираємо дані з бакетів кожного потоку
        size_t index = 0;
        for (vector<vector<long>>& buckets : thread_buckets) {
            for (vector<long>& bucket : buckets) {
                for (long num : bucket) {
                    arr[index++] = num;
                }
                bucket.clear();
            }
        }

        // Print array after sorting each digit place
        cout << "Array after sorting digit place " << digitPlace + 1 << ":" << endl;
     //   for (long num : arr) {
      //      cout << num << " ";
     //   }
    //    cout << endl;
    }

    // Merge sorted parts
   vector<long> mergedArr(arr.size());

for (int i = 1; i < num_threads; i++) {
    size_t index = i * (arr.size() / num_threads);
    merge(arr.begin(), arr.begin() + index, arr.begin() + index, arr.end(), mergedArr.begin());
    arr = mergedArr; // заміна вихідного масиву на новий відсортований масив
}
}

int main() {
    // Генеруємо випадковий масив для сортування
    vector<long> arr(1000000);
    srand(time(nullptr));
    for (long& num : arr) {
        num = rand() % 10000;
        num *= rand() % 10000;
      //  num *= rand() % 10; // Числа від 0 до 999999999 для прикладу
    }

    int num_threads = 5; // Змініть кількість потоків за потреби

    // Print original array
    cout << "Original array:" << endl;
   // for (long num : arr) {
   //     cout << num << " ";
   // }
  //  cout << endl;

    auto start = high_resolution_clock::now();
    radixSort(arr, num_threads);
    auto end = high_resolution_clock::now();

    // Перевіряємо, чи масив сортується правильно
   cout << "Sorted array:" << endl;
  // for (long num : arr) {
   //     cout << num << " ";
   // }
   // cout << endl;

    auto duration = duration_cast<milliseconds>(end - start);
    cout << "Execution time: " << duration.count() << " milliseconds" << endl;

    return 0;
}

