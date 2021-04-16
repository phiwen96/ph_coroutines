#pragma once
#include <experimental/coroutine>
#include <concepts>
#include <ph_debug/debug.hpp>
#include <vector>
#include <thread>
#include <ph_coroutines/i_am_co_awaited.hpp>
#include <ph_coroutines/i_was_co_awaited_and_now_i_am_suspending.hpp>
#include <ph_concepts/concepts.hpp>

using namespace std;
using namespace experimental;
using namespace chrono_literals;







template <typename interface_type_>
requires requires ()
{
    typename interface_type_::return_type;
}
struct co_promise
{
    using promise_type = co_promise;
    using interface_type = interface_type_;
    using return_type = typename interface_type::return_type;
   
    

    return_type m_return_value;
    
    
//    auto get_return_object () noexcept -> interface_type
//    {
//        return {coroutine_handle <promise_type>::from_promise(*this)};
//    }
    
    auto get_return_object () noexcept -> interface_type
    {
        return {static_cast <typename interface_type::promise_type&> (*this)};
    }
    
    auto initial_suspend () noexcept -> typename interface_type::initial_suspend_awaitable_type
    requires requires (){
        ph::concepts::coroutines::awaitables::initial_type <typename interface_type::initial_suspend_awaitable_type>;
    }
    {
        return {};
    }
    
//    auto final_suspend () noexcept -> typename interface_type::final_suspend_awaitable_type
//    requires requires (){
//        ph::concepts::coroutines::awaitables::final_type <typename interface_type::final_suspend_awaitable_type>;
//    }
//    {
//        return {};
//    }
    
    [[noreturn]] auto unhandled_exception () -> void
    {
//        throw runtime_error ("oops");
        terminate ();
    }
    
    /**
     co_return value;
     */
    auto return_value (auto&& value) noexcept  -> auto
    {
        
        cout << "momom" << endl;
        m_return_value = forward <decltype (value)> (value);
//        m_return_value = value;
    }
    
    /**
        i am co_awaiting another function!
     
        When we are (in our own coro-function) co_awaiting another function!
     */
//    auto await_transform (interface_type && the_function_i_co_awaited) -> typename interface_type::await_transform_awaitable_type
//    requires requires () {
//        ph::concepts::coroutines::awaitables::transform_type <typename interface_type::await_transform_awaitable_type>;
//    }
//    {
//        return {move (the_function_i_co_awaited.m_coro)};
//    }
    
    /**
     i am co_awaiting another function!
     
        When we are (in our own coro-function) co_awaiting another function!
     */
//    auto await_transform (interface_type const& the_function_i_co_awaited) -> typename interface_type::await_transform_awaitable_type
//    requires requires () {
//        ph::concepts::coroutines::awaitables::transform_type <typename interface_type::await_transform_awaitable_type>;
//    }
//    {
//        return {the_function_i_co_awaited.m_coro};
//    }

    operator return_type ()
    {
        return m_return_value;
    }
    
    auto value () -> promise_type
    {
        return m_return_value;
    }

    operator coroutine_handle <> & ()
    {
        return m_this_function_co_awaited_me;
    }
    
    operator coroutine_handle <promise_type> ()
    {
        return coroutine_handle <promise_type>::from_promise (*this);
    }
    
    coroutine_handle <> m_this_function_co_awaited_me;
};



















template <typename value_type>
struct F
{
    struct promise_type
    {
        value_type m_value;
        coroutine_handle<> m_parent;
        auto get_return_object ()
        {
            return F {coroutine_handle<promise_type>::from_promise (*this)};
        }
        auto return_value (value_type value)
        {
            m_value = value;
        }
        auto unhandled_exception ()
        {
            
        }
        auto initial_suspend ()
        {
            return suspend_always {};
        }
        auto final_suspend () noexcept
        {
            struct co_awaited
            {
                auto await_ready () noexcept
                {
                    return false;
                }
                auto await_suspend (coroutine_handle<promise_type> m_handle) noexcept -> coroutine_handle<>
                {
                    if (m_handle.promise().m_parent)
                    {
                        return m_handle.promise().m_parent;
                    } else
                    {
                        return noop_coroutine();
                    }
                }
                auto await_resume () noexcept
                {
                    
                }
            };
            return co_awaited {};
        }
        auto await_transform (F co_awaited_function)
        {
            struct i_am_coawaited
            {
                coroutine_handle<promise_type> m_handle;
                auto await_ready ()
                {
                    return false;
                }
                auto await_suspend (coroutine_handle<> parent)
                {
                    m_handle.promise().m_parent = parent;
                    return m_handle;
                }
                auto await_resume ()
                {
                    return m_handle.promise().m_value;
                }
            };
            return i_am_coawaited {co_awaited_function.m_handle};
        }
    };
    auto resume ()
    {
        return m_handle.resume();
    }
    
    F (coroutine_handle<promise_type> handle) : m_handle {handle}
    {
        
    }
    F (F&& other) : m_handle {exchange (other.m_handle, {})}
    {
        
    }
    coroutine_handle<promise_type> m_handle;
};
