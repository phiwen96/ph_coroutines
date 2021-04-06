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


//template <typename interface, typename...>
//struct co_promise;
//
//template <typename interface_type>
//struct co_promise <interface_type>
//{
//
//};

template <
    typename interface_type_
>
struct co_promise
{
    using interface_type = interface_type_;
    using value_type = typename interface_type::value_type;
    using promise_type = co_promise;
    
    struct awaitable
    {
        using initial_type =  typename interface_type::awaitable::initial_type;
        using final_type = typename interface_type::awaitable::final_type;
        using transform_type = typename interface_type::awaitable::transform_type;
    };

    value_type m_value;
    
    auto get_return_object () noexcept -> decltype (auto)
    {
        return interface_type {coroutine_handle <promise_type>::from_promise(*this)};
    }
    auto initial_suspend () noexcept -> decltype (auto)
    {
        return typename awaitable::initial_type {};
    }
    auto final_suspend () noexcept -> decltype (auto)
    {
        return typename awaitable::final_type {};
    }
    [[noreturn]] auto unhandled_exception () -> decltype (auto)
    {
        throw runtime_error ("oops");
    }
    
    /**
     co_return value;
     */
    [[nodiscard]] auto return_value (auto&& value) noexcept  -> decltype (auto)
    {
        m_value = forward <decltype (value)> (value);
    }
    
    /**
        i am co_awaiting another function!
     
        When we are (in our own coro-function) co_awaiting another function!
     */
    auto await_transform (interface_type && i_co_awaited_this_function) -> decltype (auto)
    {
        return typename awaitable::transform_type {move (i_co_awaited_this_function.m_coro)};
    }
    
    /**
     i am co_awaiting another function!
     
        When we are (in our own coro-function) co_awaiting another function!
     */
    auto await_transform (interface_type const& i_co_awaited_this_function) -> decltype (auto)
    {
        return typename awaitable::transform_type {i_co_awaited_this_function.m_coro};
    }
    
    coroutine_handle <> m_this_function_co_awaited_me;
    
    static_assert (ph::concepts::coroutines::promise_type<co_promise>, "this does not follow promise_type conventionals!");
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
