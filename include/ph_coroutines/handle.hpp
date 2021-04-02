#pragma once
#include <ph_color/color.hpp>
#include <ph_debug/debug.hpp>
#include <experimental/coroutine>
using namespace std;
using namespace std::experimental;

template <typename... promise>
struct co_handle;


template <>
struct co_handle <> : coroutine_handle <>{
    #define class_name "co_handle<>"
    using base = coroutine_handle <>;
    using base::coroutine_handle;
    using base::operator=;
    using base::operator bool;
    using base::operator();
    using base::done;
    using base::address;
    using base::from_address;
    using base::destroy;
    using base::resume;
    string called_from_function;
    int called_from_line;
    co_handle (debug_called_from) : called_from_function {move (_called_from_function)}, called_from_line {_called_from_line} {
//        debug_class_print_called_from(cyan, 0, called_from_function + "::" + to_string (called_from_line));
    }
    template <class T>
    co_handle (T&& h, debug_called_from) : base {forward<T>(h)}, called_from_function {move (_called_from_function)}, called_from_line {_called_from_line} {
//        debug_class_print_called_from(cyan, 0);
    }
    ~co_handle () {
//        cout << class_name << "::" << __FUNCTION__ << red << "OBS!! " << "potential memory leak!" << endl;
        base::~coroutine_handle ();
    }
//    auto operator co_await () {
//
//    }
};

template <typename promise>
struct co_handle <promise> : coroutine_handle <promise>{
    #define class_name "co_handle<promise>"
    using base = coroutine_handle <promise>;
//    using base::coroutine_handle;
//    using base::operator=;
    using base::operator bool;
//    using base::operator();
    using base::done;
    using base::address;
    using base::from_address;
    using base::destroy;
//    using base::resume;
    string called_from_function;
    int called_from_line;
    co_handle (debug_called_from) noexcept : base {}, called_from_function {move (_called_from_function)}, called_from_line {_called_from_line} {
//        debug_class_print_called_from (cyan, 0);
    }
//    template <class U>
    co_handle& operator= (co_handle const& other) noexcept {
        called_from_function = other.called_from_function;
        called_from_line = other.called_from_line;
        base::operator= (static_cast <coroutine_handle <promise> const&> (other));
        return *this;
    }
    co_handle& operator= (co_handle&& other) noexcept {
        swap (called_from_function, other.called_from_function);
        swap (called_from_line, other.called_from_line);
        base::operator= (static_cast <coroutine_handle <promise>&&> (other));
        return *this;
    }
//    template <typename U>
    co_handle (promise& h) noexcept : base {h.my_handle}, called_from_function {h.my_handle.called_from_function}, called_from_line {h.my_handle.called_from_line} {
        
    }
    
    /**
     someone is copying a co_handle!!
     */
    co_handle (co_handle const& h) noexcept : base {h}, called_from_function {h.called_from_function}, called_from_line {h.called_from_line} {

    }
    co_handle (co_handle && h) noexcept : base {static_cast<coroutine_handle<promise>&&>(h)}, called_from_function {move (h.called_from_function)}, called_from_line {h.called_from_line} {

    }
    co_handle (coroutine_handle <promise> const& h, debug_called_from) noexcept : base {h}, called_from_function {_called_from_function}, called_from_line {_called_from_line} {
//        if constexpr (is_same_v <U, co_handle <promise>>) {
//            cout << "kmkm" << endl;
//            co_handle me = static_cast<co_handle<promise>&>(h);
//            called_from_function = me.called_from_function;
//            called_from_line = me.called_from_line;
//        } else {
//            debug_class_print_called_from(cyan, 0);
//
//        }
    }
    co_handle (coroutine_handle <promise> && h, debug_called_from) noexcept : base {h}, called_from_function {move (_called_from_function)}, called_from_line {_called_from_line} {

    }
    static auto from_promise (promise& p, bool write_out = true, debug_called_from) -> decltype (auto) {
//        if (write_out)
//            debug_class_print_called_from(cyan, 0);
        return base::from_promise (p);
    }
    
    static co_handle <promise> from_promise (promise& p, string _called_from_function, int _called_from_line, int _called_line = __builtin_LINE ()) {
//        debug_class_print_called_from (cyan, 0)
        return {base::from_promise (p), _called_from_function, _called_line};
    }
//    co_handle (co_handle const& handle) : base::coroutine_handle {handle} {
//        cout << "yay" << endl;
//    }
    
private:
    /**
     copying handle
     */
    co_handle (coroutine_handle <promise> handle, string const& _called_from_function, int _called_from_line) : base::coroutine_handle {handle}, called_from_function {_called_from_function}, called_from_line {_called_from_line}{
//        debug_class_print_called_from (cyan, 0)
    }
};
#undef class_name
