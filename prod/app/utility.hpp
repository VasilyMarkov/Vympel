#pragma once
#include <chrono>
#include <vector>
#include <unordered_map>
#include <iostream>
#include <concepts>

template <typename T>
void print(const std::vector<T>& vector) {
    for(auto&& el:vector) {
        std::cout << el << ' ';
    }
    std::cout << std::endl;
}

template <typename T, typename U>
void print(const std::unordered_map<T, U>& map) {
    for(auto&& el:map) {
        std::cout << el << ' ';
    }
    std::cout << std::endl;
}

template <typename F>
void measureTime(F) {
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
    F();
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() << "[µs]" << std::endl;
    std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::nanoseconds> (end - begin).count() << "[ns]" << std::endl;
}
