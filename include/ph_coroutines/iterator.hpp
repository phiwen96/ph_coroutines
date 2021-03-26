#pragma once
#include <experimental/coroutine>
using namespace std;
using namespace experimental;



template <typename T>
requires requires (typename T::promise_type p, T t) {
    p.value;
}
struct generator_iter
{
    struct sentinel{};
    
    using promise_type = typename T::promise_type;
    
public:
    void operator++() {
        handle.resume();
    }
    auto operator*() -> decltype (auto) {
        return handle.promise().value;
    }
    bool operator==(sentinel) const {
        return !handle || handle.done();
    }
    explicit generator_iter (coroutine_handle <promise_type> handle) : handle{handle} {}

private:
    coroutine_handle <promise_type> handle;
};
