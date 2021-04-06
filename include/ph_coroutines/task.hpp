#pragma once
#include <experimental/coroutine>
#include <concepts>
#include <ph_debug/debug.hpp>
#include <vector>
#include <thread>

using namespace std;
using namespace experimental;
using namespace chrono_literals;


template <typename T>
struct task
{
    
    struct promise_type
    {
        #define class_name "promise_type"
        
        string m_called_from;
        coroutine_handle <> m_continuation;
        T m_value;
        
        promise_type (debug_called_from) : m_called_from {"called_from_"}
        {
            m_called_from += _called_from_function;
            m_called_from += "::";
            m_called_from += to_string (_called_from_line);
        }
    
        friend auto operator<< (ostream& os, promise_type const& p) -> ostream&
        {
            os << cyan << p.m_called_from << white;
            return os;
        }
        
        auto get_return_object () -> decltype (auto)
        {
            return task {*this};
        }
        

        auto initial_suspend () -> decltype (auto)
        {
//            cout << "initial_suspend::" << m_called_from << endl;
            struct awaitable
            {
                promise_type& m_promise;
                
                constexpr auto await_ready() const noexcept -> bool
                {
//                    cout << "\t" << green << "initial suspend::" << m_promise << endl;
                    return false;
                }
                
                constexpr auto await_suspend (coroutine_handle <void>) const noexcept -> bool
                {
                    thread {[&] () mutable {
//                        m_promise.resume();
                        coroutine_handle<promise_type>::from_promise (m_promise).resume();
                    }}.detach();

                    return true;
                }
                
                constexpr auto await_resume() const noexcept -> void
                {
                    
                }
            };
           
            return awaitable {*this};
        }
        
        constexpr auto final_suspend () noexcept -> decltype (auto)
        {
            
            struct awaitable
            {
                promise_type& m_promise;
                auto await_ready () noexcept -> bool
                {
//                    cout << "\t" << blue << "final suspend::" << m_promise << endl;
                    return false;
                }
                
                auto await_suspend (coroutine_handle <promise_type> thisCoro) noexcept -> coroutine_handle <void>
                {
                    auto& promise = thisCoro.promise();
                    if (promise.m_continuation)
                    {
                        return static_cast <coroutine_handle <void>> (promise.m_continuation);
                    }
                    return noop_coroutine();
                }
                
                auto await_resume () noexcept -> void
                {
                    
                }
            };

            return awaitable {*this};
        }
        
        auto return_value (auto&& value) -> void
        {
            m_value = forward <decltype (value)> (value);
        }
        
        auto unhandled_exception () -> void
        {
            throw runtime_error ("oops");
        }
    };
    
    auto operator co_await () const& -> decltype (auto)
    {
        struct awaitable
        {
            promise_type& m_promise;
            
            constexpr auto await_ready () noexcept -> bool
            {
                return coroutine_handle <promise_type>::from_promise (m_promise).done ();
            }
            
            constexpr auto await_suspend (coroutine_handle <void> continuation) noexcept -> bool
            {
                m_promise.m_continuation = continuation;
                return true;
            }
            
            constexpr auto await_resume () noexcept -> decltype (auto)
            {
                return m_promise.m_value;
            }
        };
        
//        cout << "co_await " << m_promise << endl;
        return awaitable {m_promise};
    }
    
    
    
    
  
    
    
    promise_type& m_promise;
    
    auto wait () -> void
    {
        while (not coroutine_handle <promise_type>::from_promise (m_promise).done ())
        {
            
        }
    }
    
    task (task const&) = delete;
    task (promise_type& promise) : m_promise {promise} {}
    task (task&& o) : m_promise {o.m_promise} {}
    ~task () {
        if (decltype (auto) coro = coroutine_handle <promise_type>::from_promise (m_promise);
            coro)
        {
            coro.destroy ();
            cout << "OK!" << endl;
        }
    }
    task& operator=(task&&) = delete;
    task& operator=(task const&) = delete;
};
