#include <iostream>
#include <vector>
#include <omp.h>
#include <iomanip>
#include <algorithm>

using namespace std;

double run(const vector<vector<int>> &data, int threads, int size) {
    double time = omp_get_wtime();

    vector<int> maxes(size);
    for (int i = 0; i < size; ++i)
        maxes[i] = data[i][0];

#pragma parallel for default(shared) num_threads(threads)
    for (int i = 0; i < size; ++i) {
        maxes[i] = *min_element(data[i].begin(), data[i].end());
    }

    vector mins(threads, maxes[0]);
#pragma parallel for default(shared) num_threads(threads)
    for (int i = 0; i < size; ++i)
        mins[omp_get_thread_num()] = mins[omp_get_thread_num()] < maxes[i] ? mins[omp_get_thread_num()] : maxes[i];

    int minmax = *min_element(mins.begin(), mins.end());
    return omp_get_wtime() - time;
}

int main() {
    int threads_max = omp_get_max_threads();
    vector times(threads_max, vector(100, 0.));
    int iter_count = 10;
    int size_max = 10'000;
    int step = 100;

    auto matrix_generator = [&size_max](int size) {
        vector data(size_max, vector(size_max, 0));
        for (auto &row: data)
            for (auto &i : row)
                i = rand();
        return move(data);
    };

    for (int i = 0; i < iter_count; ++i) {
        cout << "iter " << i +1 << "/" << iter_count << endl;
        auto data = matrix_generator(size_max);
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


/*
0.000518 0.000530 0.000837 0.001122 0.001407 0.001696 0.002007 0.002174 0.002437 0.002735 0.002978 0.003301 0.003529 0.004342 0.005360 0.005645 0.004843 0.005000 0.006389 0.007232 0.006380 0.006642 0.007997 0.008262 0.007205 0.007794 0.009134 0.008566 0.008625 0.009935 0.009204 0.009936 0.010798 0.009963 0.011943 0.010600 0.012188 0.012173 0.012656 0.012927 0.012831 0.013476 0.013045 0.014997 0.014004 0.015396 0.015149 0.013993 0.016861 0.014555 0.017296 0.016613 0.015155 0.017853 0.017600 0.017736 0.019121 0.018778 0.018902 0.018710 0.018698 0.019273 0.021414 0.021192 0.019460 0.019834 0.023117 0.023630 0.022620 0.022448 0.021783 0.022280 0.022387 0.022822 0.023453 0.023609 0.024591 0.024451 0.024997 0.025864 0.025167 0.024951 0.025418 0.025089 0.025772 0.026520 0.027383 0.026839 0.027371 0.027144 0.027765 0.027488 0.028328 0.028360 0.028773 0.029167 0.029644 0.029880 0.031807 0.031030
0.000280 0.000550 0.000869 0.001142 0.001412 0.001725 0.001988 0.002178 0.002459 0.002751 0.002984 0.003264 0.003528 0.004633 0.005372 0.005883 0.004836 0.005030 0.006619 0.007151 0.006083 0.006563 0.008026 0.008152 0.007125 0.008359 0.009631 0.008345 0.008634 0.009990 0.009295 0.010127 0.010721 0.010379 0.012082 0.010815 0.012786 0.011739 0.013138 0.012893 0.012567 0.013725 0.013181 0.014899 0.013938 0.014501 0.016100 0.013804 0.016777 0.014547 0.016500 0.017497 0.015017 0.017373 0.017452 0.017695 0.018145 0.018127 0.018149 0.018374 0.019150 0.018693 0.021116 0.021939 0.019441 0.019585 0.021882 0.022798 0.023428 0.022874 0.022312 0.022811 0.022613 0.022745 0.023211 0.023291 0.023821 0.024765 0.025354 0.024592 0.025635 0.025647 0.025380 0.025098 0.025605 0.025771 0.026025 0.027044 0.027014 0.026733 0.027337 0.027686 0.028023 0.028464 0.028641 0.028904 0.031713 0.030952 0.030335 0.029060
0.000264 0.000560 0.000843 0.001144 0.001424 0.001709 0.001961 0.002172 0.002458 0.002771 0.002991 0.003273 0.003520 0.005026 0.005450 0.005675 0.004832 0.005067 0.006952 0.007006 0.006011 0.006546 0.008034 0.008004 0.007121 0.008289 0.009502 0.008555 0.008830 0.009807 0.009377 0.010829 0.011247 0.010180 0.011993 0.010725 0.012880 0.011377 0.013631 0.012295 0.012563 0.013840 0.013658 0.014173 0.014317 0.014011 0.016453 0.014229 0.016612 0.014686 0.015778 0.018289 0.015059 0.017353 0.018042 0.017471 0.017909 0.018154 0.018837 0.019451 0.019660 0.018058 0.020384 0.021581 0.020540 0.019775 0.020745 0.021868 0.023220 0.023138 0.022942 0.022367 0.022774 0.022473 0.023241 0.023253 0.023576 0.024717 0.024711 0.024939 0.024368 0.026084 0.025925 0.025588 0.026188 0.026763 0.026767 0.026578 0.027152 0.026458 0.027051 0.027294 0.027920 0.028336 0.028923 0.030688 0.031381 0.029967 0.028979 0.029098
0.000263 0.000571 0.000861 0.001149 0.001438 0.001721 0.001924 0.002177 0.002428 0.002714 0.002987 0.003285 0.003521 0.004963 0.005395 0.005379 0.004783 0.005126 0.006825 0.006987 0.005903 0.006582 0.008036 0.007737 0.007481 0.008633 0.009221 0.008548 0.009622 0.009905 0.009023 0.010782 0.009808 0.010313 0.011685 0.010901 0.012991 0.011286 0.013565 0.012522 0.013597 0.012995 0.014382 0.014160 0.014961 0.013294 0.016380 0.013995 0.016367 0.015490 0.014806 0.017874 0.016070 0.017067 0.018444 0.018145 0.018120 0.018889 0.019271 0.019316 0.019826 0.019464 0.019585 0.021294 0.021823 0.019857 0.019785 0.021464 0.022736 0.023716 0.023814 0.022025 0.022980 0.023683 0.022248 0.023183 0.023554 0.024353 0.024998 0.025255 0.025170 0.025151 0.025938 0.026548 0.027170 0.026984 0.027605 0.026395 0.027037 0.026814 0.027071 0.027786 0.028679 0.029373 0.030690 0.030937 0.029270 0.029121 0.029440 0.028669
0.000264 0.000568 0.000861 0.001138 0.001437 0.001710 0.001924 0.002172 0.002433 0.002717 0.002978 0.003282 0.003520 0.005215 0.005428 0.005260 0.004723 0.005180 0.006810 0.006785 0.005949 0.006623 0.007996 0.007560 0.007435 0.008872 0.008959 0.008648 0.009689 0.009960 0.009008 0.010659 0.010010 0.010175 0.011891 0.010939 0.012919 0.011218 0.013451 0.012437 0.013709 0.012924 0.014516 0.013775 0.015040 0.013245 0.016123 0.014190 0.015718 0.017305 0.014592 0.017225 0.017314 0.016613 0.018560 0.018720 0.018113 0.018522 0.018169 0.018763 0.019632 0.019598 0.018909 0.020714 0.021667 0.020526 0.019887 0.020447 0.022003 0.023459 0.023451 0.023106 0.022975 0.023818 0.023908 0.022831 0.023989 0.024865 0.024442 0.024601 0.025836 0.026087 0.026038 0.025958 0.026211 0.026547 0.026443 0.026427 0.027839 0.027366 0.027997 0.028590 0.029787 0.031085 0.031099 0.029479 0.028711 0.029453 0.029923 0.028877
0.000266 0.000568 0.000866 0.001141 0.001434 0.001726 0.001919 0.002175 0.002425 0.002714 0.002989 0.003263 0.003560 0.005361 0.005365 0.004722 0.004765 0.005274 0.006873 0.006827 0.006125 0.006358 0.008064 0.007097 0.007212 0.009054 0.008733 0.008611 0.010114 0.009464 0.009322 0.010507 0.010154 0.011139 0.011390 0.010963 0.012433 0.011471 0.013198 0.012254 0.013952 0.012748 0.014711 0.012709 0.015562 0.014052 0.015769 0.014115 0.014878 0.017300 0.014796 0.016530 0.017294 0.016384 0.017808 0.018248 0.017692 0.017730 0.017890 0.018594 0.020566 0.020362 0.019409 0.019136 0.021527 0.022231 0.021152 0.019988 0.021113 0.022800 0.023056 0.022803 0.023115 0.023175 0.023592 0.024066 0.022879 0.023879 0.024611 0.024193 0.025130 0.026010 0.026616 0.026332 0.025585 0.026291 0.026595 0.027569 0.028872 0.028584 0.029147 0.029103 0.030718 0.030372 0.030020 0.028900 0.029101 0.029184 0.029309 0.031851
0.000260 0.000566 0.000879 0.001145 0.001443 0.001727 0.001907 0.002175 0.002441 0.002705 0.003020 0.003295 0.003614 0.005092 0.005391 0.004541 0.004743 0.005298 0.006943 0.006591 0.006085 0.006796 0.008000 0.006972 0.007338 0.008795 0.008690 0.008612 0.009841 0.008812 0.009636 0.010760 0.009535 0.011280 0.011289 0.010817 0.012144 0.011845 0.013011 0.012223 0.013698 0.012557 0.014560 0.012753 0.015391 0.014019 0.015107 0.015214 0.014317 0.017218 0.014926 0.016383 0.017900 0.017549 0.017873 0.017996 0.017854 0.018048 0.018577 0.018750 0.020218 0.020837 0.019728 0.019128 0.020962 0.022700 0.022626 0.020075 0.020624 0.021958 0.022583 0.023084 0.022748 0.023203 0.023129 0.023970 0.024655 0.023834 0.023671 0.024468 0.025625 0.025969 0.026132 0.027317 0.026372 0.026992 0.027912 0.028323 0.027788 0.028070 0.028596 0.028667 0.029242 0.028143 0.028024 0.028787 0.028221 0.028256 0.031208 0.033322
0.000258 0.000562 0.000850 0.001131 0.001436 0.001684 0.001903 0.002170 0.002435 0.002705 0.003025 0.003265 0.003728 0.005126 0.005536 0.004507 0.004775 0.005567 0.006908 0.006257 0.006232 0.007462 0.007960 0.006906 0.007425 0.009032 0.008257 0.008384 0.009797 0.009114 0.009325 0.010412 0.009547 0.011411 0.010737 0.011354 0.011754 0.011897 0.012741 0.012299 0.013802 0.012827 0.014844 0.013157 0.015410 0.014371 0.014381 0.015948 0.014247 0.016901 0.015019 0.015751 0.018277 0.018030 0.017630 0.018206 0.018386 0.018472 0.018959 0.018463 0.019640 0.021674 0.019674 0.018842 0.020483 0.022162 0.022790 0.022243 0.020180 0.021190 0.021983 0.023110 0.022656 0.022653 0.023340 0.024043 0.024203 0.025174 0.024534 0.024310 0.024983 0.025454 0.025787 0.026366 0.026475 0.026977 0.027429 0.027227 0.027000 0.027742 0.028057 0.028631 0.028140 0.028162 0.028564 0.028809 0.027259 0.030460 0.032008 0.031651
*/