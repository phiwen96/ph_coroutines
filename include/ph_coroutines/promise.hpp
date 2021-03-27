#pragma once
#include <experimental/coroutine>
#include <concepts>

using namespace std;
using namespace experimental;
using namespace std::chrono_literals;



//struct default_sentinel_t { };



template <template <class...> class Generator, template <class...> class Iterator, class T>
struct promise
{
    using value_type = T;
    using iterator = Iterator <promise>;
    using generator = Generator <promise>;
    T value;

    promise () {
        cout << "promise()" << endl;
    }
    ~promise () {
        cout << "~promise()" << endl;
    }
    auto get_return_object () -> decltype (auto) {
        return generator {coroutine_handle<promise>::from_promise(*this)};
    }
    static constexpr auto initial_suspend () noexcept {
        return suspend_always{};
    }
    static constexpr auto final_suspend () noexcept {
        return suspend_always{};
    }
    [[nodiscard]] auto yield_value (auto&& u) noexcept {
        value = forward <decltype (u)> (u);
        return suspend_always{};
    }
    constexpr void return_void () {}
    void await_transform() = delete;
    [[noreturn]] static void unhandled_exception () {
        throw;
//            terminate();
    }
    [[nodiscard]] static void* operator new (size_t sz) {
//            cout << "allocation heap for coroutine" << endl;//: " << sz << " bytes" << endl;
        return ::operator new (sz);
    }
    [[nodiscard]] static void operator delete (void* ptr) {
        ::operator delete (ptr);
//            cout << "deallocating heap for coroutine" << endl;
    }
};
