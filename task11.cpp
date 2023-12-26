#include <iostream>
#include <vector>
#include <omp.h>
#include <iomanip>
#include <algorithm>

using namespace std;

double run_static(const vector<int> &data, int threads, int size) {
    double time = omp_get_wtime();
    omp_set_num_threads(threads);

#pragma omp parallel for schedule(static)
    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < data[i]; j++) {
            vector t(100,0);
            for (auto &i : t)
                i = rand();
        }
    }

    return omp_get_wtime() - time;
}

double run_dynamic(const vector<int> &data, int threads, int size) {
    double time = omp_get_wtime();
    omp_set_num_threads(threads);

#pragma omp parallel for schedule(dynamic)
    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < data[i]; j++) {
            vector t(50,0);
            for (auto &i : t)
                i = rand();
        }
    }

    return omp_get_wtime() - time;
}

double run_guided(const vector<int> &data, int threads, int size) {
    double time = omp_get_wtime();
    omp_set_num_threads(threads);

#pragma omp parallel for schedule(guided)
    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < data[i]; j++) {
            vector t(100,0);
            for (auto &i : t)
                i = rand();
        }
    }

    return omp_get_wtime() - time;
}

double run_runtime(const vector<int> &data, int threads, int size) {
    double time = omp_get_wtime();
    omp_set_num_threads(threads);

#pragma omp parallel for schedule(runtime)
    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < data[i]; j++) {
            vector t(100,0);
            for (auto &i : t)
                i = rand();
        }
    }

    return omp_get_wtime() - time;
}

int main() {
    int threads_max = omp_get_max_threads();
    vector times_static(threads_max, 0.);
    vector times_dynamic(threads_max, 0.);
    vector times_guided(threads_max, 0.);
    vector times_runtime(threads_max, 0.);

    int iter_count = 10;
    int size_max = 10'000;

    auto triang_generator = [](int size) {
        vector data(size, 0);
        for (int i = 0; i < size; ++i) {
            data[i] = i/50 + rand()%10 + 1;
        }
        return data;
    };

    for (int i = 0; i < iter_count; ++i) {
        cout << "iter " << i +1 << "/" << iter_count << endl;
        auto triang = triang_generator(size_max);
        for (int threads = 1; threads <= threads_max; ++threads) {
            cout << "\t\tthreads " << threads << "/" << threads_max << endl;
            times_dynamic[threads-1] += run_dynamic(triang, threads, size_max);
            times_static[threads-1] += run_static(triang, threads, size_max);
            times_guided[threads-1] += run_guided(triang, threads, size_max);
            times_runtime[threads-1] += run_runtime(triang, threads, size_max);
        }
    }

    cout << endl;
    std::cout << std::fixed;
    std::cout << std::setprecision(6);

    auto print = [iter_count](const vector<double>& vec, const string &str) {
        cout << str << endl;
        for (auto i : vec)
            cout << i / iter_count << ", ";
        cout << endl;
    };

    print(times_static, "static");
    print(times_dynamic, "dynamic");
    print(times_guided,"guided");
    print(times_runtime, "runtime");
    return 0;
}


/*
static
1.886680, 4.075454, 4.867252, 5.234580, 5.981967, 6.126295, 6.455676, 6.840588,
dynamic
0.948239, 3.227461, 2.529896, 2.956713, 3.528118, 3.776591, 3.755452, 3.953782,
guided
1.878020, 6.223000, 5.200408, 5.900786, 7.022993, 7.543024, 7.410260, 7.822724,
runtime
1.878690, 6.136566, 5.184628, 5.913829, 7.024097, 7.490899, 7.343423, 7.775164,
*/