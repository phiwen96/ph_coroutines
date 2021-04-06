#pragma once
#include <experimental/coroutine>
#include <concepts>
#include <ph_debug/debug.hpp>
#include <vector>
#include <thread>

using namespace std;
using namespace experimental;
using namespace chrono_literals;


/**
 @brief When our function is done executing, it should
        resume the function that co_awaited us!
 */
struct i_was_co_awaited_and_now_i_am_suspending
{
    auto await_ready () noexcept -> bool
    {
        return false;
    }
    
    /**
     @param my_handle is a handle to this function
     
     @return a handle to the function that co_awaited us.
     */
    template <typename promise_type>
    auto await_suspend (coroutine_handle <promise_type> my_handle) noexcept -> coroutine_handle <>
    {
        auto& parent = my_handle.promise().m_this_function_co_awaited_me;
        return parent ? parent : noop_coroutine ();
    }
    
    auto await_resume () noexcept
    {
        
    }
};
