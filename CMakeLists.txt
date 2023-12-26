cmake_minimum_required(VERSION 3.22)
project(task1)

set(CMAKE_CXX_STANDARD 17)

add_executable(task1 task1.cpp)
add_executable(task2 task2.cpp)
add_executable(task3 task3.cpp)
add_executable(task4 task4.cpp)
add_executable(task5 task5.cpp)
add_executable(task6 task6.cpp)
add_executable(task9 task9.cpp)
add_executable(task10 task10.cpp)
add_executable(task11 task11.cpp)


find_package(OpenMP)

if(OpenMP_CXX_FOUND)
    target_link_libraries(task1 PUBLIC OpenMP::OpenMP_CXX)
    target_link_libraries(task2 PUBLIC OpenMP::OpenMP_CXX)
    target_link_libraries(task3 PUBLIC OpenMP::OpenMP_CXX)
    target_link_libraries(task4 PUBLIC OpenMP::OpenMP_CXX)
    target_link_libraries(task5 PUBLIC OpenMP::OpenMP_CXX)
    target_link_libraries(task6 PUBLIC OpenMP::OpenMP_CXX)
    target_link_libraries(task9 PUBLIC OpenMP::OpenMP_CXX)
    target_link_libraries(task10 PUBLIC OpenMP::OpenMP_CXX)
    target_link_libraries(task11 PUBLIC OpenMP::OpenMP_CXX)

endif()
