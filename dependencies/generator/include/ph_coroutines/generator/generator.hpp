#pragma once
#include <experimental/coroutine>
#include <concepts>
#include "iterator.hpp"
//#include <ph_coroutines/coroutines.hpp>

//using namespace std;
using namespace std::experimental;
using namespace std::chrono_literals;




template <class promise>
struct generator
{
    
    using value_type = typename promise::value_type;
    
    using promise_type = promise;
    coroutine_handle <promise_type> handle;
    
    explicit generator (coroutine_handle <promise_type> h) : handle {h} {
        cout << "generator(handle)" << endl;
    }
    generator (generator&& other) : handle {exchange (other.handle, {})} {
        cout << "generator(other)" << endl;
    }
    generator& operator=(generator&& other) noexcept {
           if (this != &other) {
               if (handle) {
                   handle.destroy();
               }
               swap (handle, other.handle);
           } else
           {
               throw std::runtime_error ("coro");
           }
           return *this;
       }
    ~generator () {
        cout << "~generator()" << endl;
        // destroy the coroutine
        if (handle)
        {
            handle.destroy();
        }
    }
    generator(generator const&) = delete;
    generator& operator=(const generator&) = delete;
    explicit operator bool() {
            return !handle.done();
    }
    
    template <typename O>
    requires requires (value_type t, O o) {
        o = t;
    }
    operator O () {
        if (handle.done())
        {
            throw std::runtime_error ("no, coroutine is done");
        }
        
        handle.resume();
        return handle.promise().value;
    }
    
    auto operator () () -> decltype (auto) {
        // will allocate a new stack frame and store the return-address of the caller in the
        // stack frame before transfering execution to the function.
        
        // resumes the coroutine from the point it was suspended
        if (handle.done())
        {
            throw std::runtime_error ("no, coroutine is done");
        }
        
        handle.resume();
        return handle.promise().value;
    }

    using iterator = typename promise_type::iterator;
    
    auto begin() -> decltype (auto) {
        if (handle) {
            handle.resume();
        }
        return iterator {handle};
    }
    auto end() -> decltype (auto) {
        return typename iterator::sentinel{};
    }

};
