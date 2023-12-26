#include <iomanip>
#include <iostream>
#include <vector>
#include <algorithm>
#include <omp.h>
using namespace std;

double run(const vector<int> &data, int threads, int size) {
    double time = omp_get_wtime();
    omp_set_num_threads(threads);
    vector maxes(threads, data[0]);
#pragma omp parallel for default(shared)
    for (int i = 0; i < size; ++i)
        if (data[i] < maxes[omp_get_thread_num()])
            maxes[omp_get_thread_num()] = data[i];

    int max = *max_element(maxes.begin(), maxes.end());

    return omp_get_wtime() - time;
}

int main(int argc, char** argv) {
    int threads_max = omp_get_max_threads();
    vector times(threads_max, vector(100, 0.));

    auto vector_generator = [](int size) {
        vector data(1'000'000'000, 0);
        for (auto &i: data)
            i = rand();
        return move(data);
    };

    int iter_count = 10;
    int size_max = 100'000'000;
    int step = 1'000'000;

    for (int i = 0; i < iter_count; ++i) {
        cout << "iter " << i +1 << "/" << iter_count << endl;
        auto data = vector_generator(size_max);
        for (int size = step; size <= size_max; size += step) {
            cout << "\tsize " << size << "/" << size_max << endl;
            for (int threads = 1; threads <= threads_max; ++threads) {
                cout << "\t\tthreads " << threads << "/" << threads_max << endl;
                times[threads-1][size/step-1] += run(data, threads, size);
            }
        }
    }

    cout << endl;
    std::cout << std::fixed;
    std::cout << std::setprecision(6);
    for (const auto& thread : times) {
        for (const auto time : thread) {
            cout << time / iter_count << " ";
        }
        cout << endl;
    }
    return 0;
}