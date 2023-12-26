#include <iostream>
#include <vector>
#include <omp.h>
#include <iomanip>
#include <algorithm>

using namespace std;

double run_A(const vector<vector<int>> &data, int threads, int size) {
    double time = omp_get_wtime();
    vector<int> maxes(size);
    for (int i = 0; i < size; ++i)
        maxes[i] = data[i][0];

#pragma parallel for default(shared)
    for (int i = 0; i < size; ++i) {
        maxes[i] = *min_element(data[i].begin(), data[i].end());
    }

    vector mins(threads, maxes[0]);
#pragma parallel for default(shared)
    for (int i = 0; i < size; ++i)
        mins[omp_get_thread_num()] = mins[omp_get_thread_num()] < maxes[i] ? mins[omp_get_thread_num()] : maxes[i];

    int minmax = *min_element(mins.begin(), mins.end());
    return omp_get_wtime() - time;
}

double run_B(const vector<vector<int>> &data, int threads, int size) {
    double time = omp_get_wtime();
    vector<int> maxes(size);
    for (int i = 0; i < size; ++i)
        maxes[i] = data[i][0];

    for (int i = 0; i < size; ++i) {
        vector tmaxes(threads, data[i][0]);
#pragma parallel for default(shared)
        for (int j = 0; j < size; ++j)
            if (data[i][j] > tmaxes[omp_get_thread_num()])
                tmaxes[omp_get_thread_num()] = data[i][j];

        maxes[i] = *max_element(tmaxes.begin(), tmaxes.end());
    }

    vector mins(threads, maxes[0]);
#pragma parallel for default(shared)
    for (int i = 0; i < size; ++i)
        mins[omp_get_thread_num()] = mins[omp_get_thread_num()] < maxes[i] ? mins[omp_get_thread_num()] : maxes[i];

    int minmax = *min_element(mins.begin(), mins.end());
    return omp_get_wtime() - time;
}

int main() {
    int threads_max = omp_get_max_threads();
    vector times_A(threads_max, 0.);
    vector times_B(threads_max, 0.);
    int iter_count = 10;
    int size = 10'000;
    int step = 100;

    auto matrix_generator = [](int size) {
        vector data(size, vector(size, 0));
        for (auto &row: data)
            for (auto &i : row)
                i = rand();
        return move(data);
    };

    for (int i = 0; i < iter_count; ++i) {
        cout << "iter " << i +1 << "/" << iter_count << endl;
        auto data = matrix_generator(size);
        for (int threads = 1; threads <= threads_max; ++threads) {
            cout << "\t\tthreads " << threads << "/" << threads_max << endl;
            times_A[threads-1] += run_A(data, threads, size);
            times_B[threads-1] += run_B(data, threads, size);
        }
    }

    cout << endl;
    std::cout << std::fixed;
    std::cout << std::setprecision(6);
    cout << "A" << endl;
    for (const auto& t : times_A) {
            cout << t / iter_count << " ";
    }
    cout << endl;

    cout << "B" << endl;
    for (const auto& t : times_B) {
        cout << t / iter_count << " ";
    }
    cout << endl;

    return 0;
}


/*
A
0.029674 0.031131 0.028394 0.030388 0.030011 0.029754 0.028515 0.028254
B
0.228064 0.232192 0.222503 0.228810 0.225007 0.224318 0.221571 0.222605
*/