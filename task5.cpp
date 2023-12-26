#include <iostream>
#include <vector>
#include <omp.h>
#include <iomanip>
#include <algorithm>

using namespace std;

double run(vector<vector<int>> &data, int threads, int size) {
    double time = omp_get_wtime();
    vector<int> maxes(size);
    for (int i = 0; i < size; ++i)
        maxes[i] = data[i][0];

#pragma parallel for default(shared) num_threads(threads)
    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j)
            if (data[i][j] > maxes[i])
                maxes[i] = data[i][j];
    }

    int minmax = maxes[0];
    for (auto i : maxes)
        minmax = i < minmax ? i : minmax;
    return omp_get_wtime() - time;
}

int main() {
    int threads_max = omp_get_max_threads();
    vector times1(threads_max, 0.);
    vector times2(threads_max, 0.);
    int iter_count = 10;
    int size_max = 10'000;
    int step = 100;

    auto triang_generator = [](int size) {
        vector data(size, vector(size, 0));
        for (int i = 0; i < size; ++i) {
            for (int j = 0; j <= i; ++j)
                data[i][j] = rand() % 10000 - 5000;
        }
        return data;
    };

    auto band_generator = [](int size) {
        vector data(size, vector(size, 0));
        int w = 10;
        for (int i = 0; i < size; ++i) {
            for (int j = min(0, i-w); j < max(size, i+w); ++j)
                data[i][j] = rand();
        }
        return data;
    };

    for (int i = 0; i < iter_count; ++i) {
        cout << "iter " << i +1 << "/" << iter_count << endl;
        vector<vector<int>> triang = vector<vector<int>>(size_max, vector<int>(size_max, 0));
        vector<vector<int>> band = vector<vector<int>>(size_max, vector<int>(size_max, 0));

        for (int i = 0; i < size_max; ++i) {
            for (int j = 0; j <= i; ++j)
                triang[i][j] = rand() % 10000 - 5000;
        }

        int w = 10;
        for (int i = 0; i < size_max; ++i) {
            for (int j = min(0, i-w); j < max(size_max, i+w); ++j)
                band[i][j] = rand() % 10000 - 5000;
        }
        for (int threads = 1; threads <= threads_max; ++threads) {
            cout << "\t\tthreads " << threads << "/" << threads_max << endl;
            times1[threads-1] += run(ref(triang), threads, size_max);
            times2[threads-1] += run(ref(band), threads, size_max);
        }
    }

    cout << endl;
    std::cout << std::fixed;
    std::cout << std::setprecision(6);

    cout << "triang" << endl;
    for (auto i : times1)
        cout << i << " ";
    cout << endl;

    cout << "band" << endl;
    for (auto i : times2)
        cout << i << " ";
    cout << endl;

    return 0;
}


/*

*/