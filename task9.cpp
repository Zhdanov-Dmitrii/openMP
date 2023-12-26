#include <iomanip>
#include <iostream>
#include <vector>
#include <omp.h>
#include <thread>
#include <unordered_map>
#include <functional>

using namespace std;

double run_A(const vector<vector<int>> &m1, const vector<vector<int>> &m2, int threads) {
    double time = omp_get_wtime();
    vector<vector<int>> res(m1.size(), vector<int>(m1.size()));
    omp_set_num_threads(threads);

#pragma omp parallel for default(shared)
    for (int i = 0; i < m1.size(); ++i) {           // A
        for (int j = 0; j < m1.size(); ++j) {       // B
            for (int k = 0; k < m1.size(); ++k) {   // C
                res[i][j] = m1[i][k] * m2[k][j];
            }
        }
    }

    return omp_get_wtime() - time;
}

double run_B(const vector<vector<int>> &m1, const vector<vector<int>> &m2, int threads) {
    double time = omp_get_wtime();
    vector<vector<int>> res(m1.size(), vector<int>(m1.size()));
    omp_set_num_threads(threads);


    for (int i = 0; i < m1.size(); ++i) {           // A
#pragma omp parallel for default(shared)
        for (int j = 0; j < m1.size(); ++j) {       // B
            for (int k = 0; k < m1.size(); ++k) {   // C
                res[i][j] = m1[i][k] * m2[k][j];
            }
        }
    }

    return omp_get_wtime() - time;
}

double run_C(const vector<vector<int>> &m1, const vector<vector<int>> &m2, int threads) {
    double time = omp_get_wtime();
    vector<vector<int>> res(m1.size(), vector<int>(m1.size()));
    omp_set_num_threads(threads);


    for (int i = 0; i < m1.size(); ++i) {           // A
        for (int j = 0; j < m1.size(); ++j) {       // B
            int curr = res[i][j];
#pragma omp parallel for default(shared) reduction(+:curr)
            for (int k = 0; k < m1.size(); ++k) {   // C
                curr = m1[i][k] * m2[k][j];
            }
            res[i][j] = curr;
        }
    }

    return omp_get_wtime() - time;
}

double run_AB(const vector<vector<int>> &m1, const vector<vector<int>> &m2, int threads) {
    double time = omp_get_wtime();
    vector<vector<int>> res(m1.size(), vector<int>(m1.size()));
    omp_set_num_threads(threads);

#pragma omp parallel for default(shared)
    for (int i = 0; i < m1.size(); ++i) {           // A
#pragma omp parallel for default(shared)
        for (int j = 0; j < m1.size(); ++j) {       // B
            for (int k = 0; k < m1.size(); ++k) {   // C
                res[i][j] = m1[i][k] * m2[k][j];
            }
        }
    }

    return omp_get_wtime() - time;
}

double run_BC(const vector<vector<int>> &m1, const vector<vector<int>> &m2, int threads) {
    double time = omp_get_wtime();
    vector<vector<int>> res(m1.size(), vector<int>(m1.size()));
    omp_set_num_threads(threads);


    for (int i = 0; i < m1.size(); ++i) {           // A
#pragma omp parallel for default(shared)
        for (int j = 0; j < m1.size(); ++j) {       // B
            int curr = res[i][j];
#pragma omp parallel for default(shared) reduction(+:curr)
            for (int k = 0; k < m1.size(); ++k) {   // C
                curr = m1[i][k] * m2[k][j];
            }
            res[i][j] = curr;
        }
    }

    return omp_get_wtime() - time;
}

double run_AC(const vector<vector<int>> &m1, const vector<vector<int>> &m2, int threads) {
    double time = omp_get_wtime();
    vector<vector<int>> res(m1.size(), vector<int>(m1.size()));
    omp_set_num_threads(threads);

#pragma omp parallel for default(shared)
    for (int i = 0; i < m1.size(); ++i) {           // A
        for (int j = 0; j < m1.size(); ++j) {       // B
            int curr = res[i][j];
#pragma omp parallel for default(shared) reduction(+:curr)
            for (int k = 0; k < m1.size(); ++k) {   // C
                curr = m1[i][k] * m2[k][j];
            }
            res[i][j] = curr;
        }
    }

    return omp_get_wtime() - time;
}

double run_ABC(const vector<vector<int>> &m1, const vector<vector<int>> &m2, int threads) {
    double time = omp_get_wtime();
    vector<vector<int>> res(m1.size(), vector<int>(m1.size()));
    omp_set_num_threads(threads);

#pragma omp parallel for default(shared)
    for (int i = 0; i < m1.size(); ++i) {           // A
#pragma omp parallel for default(shared)
        for (int j = 0; j < m1.size(); ++j) {       // B
            int curr = res[i][j];
#pragma omp parallel for default(shared) reduction(+:curr)
            for (int k = 0; k < m1.size(); ++k) {   // C
                curr = m1[i][k] * m2[k][j];
            }
            res[i][j] = curr;
        }
    }

    return omp_get_wtime() - time;
}

int main(int argc, char **argv) {
    int threads_max = omp_get_max_threads();
    unordered_map<string, function<double(const vector<vector<int>> &, const vector<vector<int>> &, int)>> funcs{
            {"A",   run_A},
            {"B",   run_B},
            {"C",   run_C},
            {"AB",  run_AB},
            {"BC",  run_BC},
            {"AC",  run_AC},
            {"ABC", run_ABC}
    };

    int iter_count = 10;
    int size_max = 1'000;

    auto matr_generator = [](int size) {
        vector data(size, vector(size, 0));
        for (auto &i: data)
            for (auto &j: i)
                j = rand() % 1000;
        return move(data);
    };

    auto times = unordered_map<string, vector<double>> {
            {"A",   vector<double>(threads_max)},
            {"B",   vector<double>(threads_max)},
            {"C",   vector<double>(threads_max)},
            {"AB",  vector<double>(threads_max)},
            {"BC",  vector<double>(threads_max)},
            {"AC",  vector<double>(threads_max)},
            {"ABC", vector<double>(threads_max)}
    };

    for (int i = 0; i < iter_count; ++i) {
        auto m1 = matr_generator(size_max);
        auto m2 = matr_generator(size_max);

        for (int threads = 1; threads <= threads_max; ++threads) {
            cout << "\tthreads " << threads << endl;
            for (auto &[name, func] : funcs) {
                cout << "\t\t" << name << endl;
                times[name][threads-1] += func(m1, m2, threads);
            }
        }
    }

    for (auto& [name, data] : times) {
        cout << name << endl;
        for (auto &el : data)
            cout << el / iter_count << " ";
        cout << endl;
    }

    return 0;
}

/*
ABC
0.679753, 0.386954, 0.292251, 0.238354, 0.20475, 0.200519, 0.177754, 0.167352
BC
0.670502, 0.407395, 0.299609, 0.244186, 0.214756, 0.202045, 0.192606, 0.224648
AB
1.70583, 0.809003, 0.521331, 0.400613, 0.531116, 0.545591, 0.53048, 0.521974
B
1.70269, 0.870965, 0.563624, 0.428656, 0.80176, 0.754954, 0.692086, 0.699101
AC
0.666883, 0.384056, 0.272247, 0.219914, 0.202785, 0.17679, 0.154263, 0.148682
C
0.436321, 0.748558, 0.885592, 0.968492, 1.14665, 1.18761, 1.41221, 2.28209
A
1.70544, 0.810295, 0.525292, 0.394237, 0.518488, 0.505082, 0.534041, 0.5078
*/