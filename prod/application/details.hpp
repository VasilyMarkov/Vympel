#ifndef DETAILS_H
#define DETAILS_H

#include <iostream>
#include <type_traits>
#include <concepts>
#include <chrono>
#include <utility>
#include <functional>

namespace details
{
namespace {

template<std::invocable Callable>
class [[nodiscard]] Defer {
public:
    Defer(Callable&& func) : func_(std::move(func)) {}
    ~Defer() {func_();}
private:
    Callable func_;
};

template<typename T>
concept IsPrintable = requires(T val, std::ostream& os) {
    { os << val } -> std::convertible_to<std::ostream&>;
};

} //namespace

template<typename... Args, std::invocable<Args...> Callable>
decltype(auto) time_execution(Callable&& function, Args&&... args) noexcept(std::is_nothrow_invocable_v<Callable, Args...>) {
    using clock = std::chrono::high_resolution_clock;
    Defer timeit([start = clock::now()](){
        try {
            std::printf("Execution time is %d seconds", std::chrono::duration<double>(clock::now() - start).count());
        } catch(const std::exception& e) {
            std::cout << e.what() << std::endl;
        }
    });
    return std::invoke(std::forward<Callable>(function), std::forward<Args>(args)...);;
}

template<typename Cont>
requires std::forward_iterator<typename Cont::const_iterator>
void print(const Cont& container) {
    if constexpr(requires { typename Cont::key_type; typename Cont::mapped_type; }) {
        for(const auto& [key, element] : container) {
            std::cout << key << ": " << element << "; ";
        }
    }
    else {
        for(const auto& element : container) {
            std::cout << element << ", ";
        }
    }
    std::cout << std::endl;
}

} //namespace details


#endif //DETAILS_H