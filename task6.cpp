#include <iomanip>
#include <iostream>
#include <vector>
#include <omp.h>
#include <thread>

using namespace std;

double run_reduction(const vector<int> &vec1, const vector<int> &vec2, int threads, int size) {
    double time = omp_get_wtime();
    int res = 0;
    omp_set_num_threads(threads);
#pragma omp parallel for default(shared) reduction(+:res)
    for (int i = 0; i < size; ++i)
        res += vec1[i]*vec2[i];

    return omp_get_wtime() - time;
}

double run_atomic(const vector<int> &vec1, const vector<int> &vec2, int threads, int size) {
    double time = omp_get_wtime();
    int res = 0;
    omp_set_num_threads(threads);
#pragma omp parallel for default(shared)
    for (int i = 0; i < size; ++i)
#pragma omp atomic
        res += vec1[i]*vec2[i];

    return omp_get_wtime() - time;
}

double run_lock(const vector<int> &vec1, const vector<int> &vec2, int threads, int size) {
    double time = omp_get_wtime();
    int res = 0;
    omp_set_num_threads(threads);
    omp_lock_t lock;
    omp_init_lock(&lock);
#pragma omp parallel for default(shared)
    for (int i = 0; i < size; ++i) {
        omp_set_lock(&lock);
        res += vec1[i] * vec2[i];
        omp_unset_lock(&lock);
    }
    return omp_get_wtime() - time;
}

double run_lin(const vector<int> &vec1, const vector<int> &vec2, int threads, int size) {
    double time = clock();
    int res = 0;
    omp_set_num_threads(threads);
    for (int i = 0; i < size; ++i)
        res += vec1[i]*vec2[i];

    return (clock() - time) / 1000.;
}

double run_critical(const vector<int> &vec1, const vector<int> &vec2, int threads, int size) {
    double time = omp_get_wtime();
    int res = 0;
    omp_set_num_threads(threads);
#pragma omp parallel for default(shared)
    for (int i = 0; i < size; ++i)
#pragma omp critical
        res += vec1[i]*vec2[i];

    return omp_get_wtime() - time;
}

int main(int argc, char **argv) {
    int threads_max = omp_get_max_threads();
    vector times_reduction(threads_max, 0.);
    vector times_critical(threads_max, 0.);
    vector times_atomic(threads_max, 0.);
    vector times_lock(threads_max, 0.);

    auto vector_generator = [](int size) {
        vector data(1'000'000'000, 0);
        for (auto &i: data)
            i = rand() % 10'000;
        return move(data);
    };

    int iter_count = 1;
    int size_max = 100'000'000;

    for (int i = 0; i < iter_count; ++i) {
        cout << "iter " << i +1 << "/" << iter_count << endl;
        auto vec1 = vector_generator(size_max);
        auto vec2 = vector_generator(size_max);
        for (int threads = 1; threads <= threads_max; ++threads) {
            cout << "\tthreads " << threads << endl;
            times_atomic[threads-1] += run_atomic(vec1, vec2, threads, size_max);
            times_critical[threads-1] += run_critical(vec1, vec2, threads, size_max);
            times_lock[threads-1] += run_lock(vec1, vec2, threads, size_max);
            times_reduction[threads-1] += run_reduction(vec1, vec2, threads, size_max);
        }
    }

    auto print_vec = [](const vector<double> &vec, const string& str) {
        cout << str << endl;
        for (auto i : vec)
            cout << i << " ";
        cout << endl;
    };
    cout << endl;
    std::cout << std::fixed;
    std::cout << std::setprecision(6);

    print_vec(times_reduction, "reduction");
    print_vec(times_atomic, "atomic");
    print_vec(times_critical, "critical");
    print_vec(times_lock, "lock");

    return 0;
}

/*
reduction
0.043233 0.031940 0.031057 0.031822 0.033547 0.033174 0.034056 0.035785
atomic
3.917017 1.062731 1.168231 1.214975 1.174259 1.112667 1.078828 1.066477
critical
1.530382 5.338007 6.785203 9.567389 9.508735 10.369723 11.843145 19.062535
lock
1.533569 6.877820 13.254255 14.487020 16.825716 19.114753 22.401285 24.796006
 */