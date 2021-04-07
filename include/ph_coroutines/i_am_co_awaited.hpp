#pragma once
#include <experimental/coroutine>
#include <concepts>
#include <ph_debug/debug.hpp>
#include <vector>
#include <thread>
#include <ph_concepts/concepts.hpp>
#include "promise.hpp"
//#include "test.out.hpp"

using namespace std;
using namespace experimental;
using namespace chrono_literals;
//NAMESPACE_STD
//#include "../../src/common.hpp"
#define class_name "i_am_co_awaited"
//using_namespaces




/**
 @brief I am co_awaited by another coroutine!
 
        1) What to do WHEN another function co_awaits us.
        2) What to do WITH the function that co_awaited us.
        3) What to do AFTER our choice is executed.
 
        Someone co_awaits us. So, lets save a handle to it and then
        begin execution of our self. 
 */
template <typename promise_type>
//requires requires (promise_type p) {
//    typename promise_type::value_type;
//    p.m_value;
//}

struct i_am_co_awaited
{
    using value_type = typename promise_type::value_type;
//    using promise_type = typename interface_type::promise_type;
    coroutine_handle <promise_type> m_function_handle;
    
    
    /**
     @brief Are we ready for execution immediately
            when someone co_awaits us?
            
            Yes, but first we need a handle to the suspending
            function (parent). We may want to continue its
            execution.
     
     @return bool, Whether we are ready for execution!
     */
    auto await_ready () -> int
    {
//        d1 (yellow, 0, "my function name: " + m_handle.promise().m_function_name)
//        return not m_handle.done();
        return false;
    }
    
    
    
    
    /**
     @brief When another function calls us with co_await,
            we want to execute our function!
     
            We alse want to save a handle to the suspending
            function so that we can resume it later on!
     
     
     @param parent function that co_await'ed us. The suspending function.

     @return a function execution, the function that should begin/continue execution!
     */
    auto await_suspend (coroutine_handle <> this_function_co_awaited_me) -> coroutine_handle <promise_type>
    requires requires (promise_type& p) {
        (coroutine_handle <> &) p;
    }
    {
        coroutine_handle<>& co_awaited_from = m_function_handle.promise();
        co_awaited_from = this_function_co_awaited_me;
        return m_function_handle;
    }
    
    
    
    
    
    /**
     @brief What to do before resuming the function that co_await'ed us!
            (AKA parent coro).
     */
    auto await_resume () -> value_type
    {
        return m_function_handle.promise();
    }
};









//template <typename my_function_handle>
//i_am_co_awaited (my_function_handle) -> i_am_co_awaited <my_function_handle>;



#undef class_name
