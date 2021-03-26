#pragma once
#include <experimental/coroutine>
using namespace std;
using namespace experimental;



template <typename promise>
//requires requires (typename T::promise_type p, T t) {
//    p.value;
//}
struct gen_iterator
{
    struct sentinel{};
    
    using promise_type = promise;
    
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
    explicit gen_iterator (coroutine_handle <promise_type> handle) : handle{handle} {}

private:
    coroutine_handle <promise_type> handle;
};
