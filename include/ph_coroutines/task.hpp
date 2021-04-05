#include <experimental/coroutine>
#include <concepts>
#include <ph_debug/debug.hpp>


using namespace std;
using namespace experimental;
using namespace chrono_literals;


atomic<int> ccount {0};

template <typename T, bool _multithreading = true>
struct task {
    
    struct promise_type {
        inline static constexpr bool multithreading = _multithreading;
        bool parent_is_not_multithreading_so_join_thread = not multithreading;
        promise_type (debug_called_from) : m_called_from {_called_from_function} {
            
        }
        string m_called_from;
        int m_ccount = ccount++;
        coroutine_handle <> m_continuation;
        T m_value;
        
        auto get_return_object () {
            return task {*this};
        }
        
        auto initial_suspend () {
//            cout << "initial_suspend::" << m_called_from << endl;
            struct awaitable {
                promise_type& m_promise;
                bool await_ready() const noexcept {
                    if constexpr (multithreading)
                    {
                        cout << m_promise.m_called_from << " oja" << endl;
//                        return false;
                        
                    } else
                    {
                        cout << m_promise.m_called_from << " onej" << endl;

//                        return true;
                    }
                    return false;
                }
                constexpr void await_suspend(coroutine_handle<>) const noexcept {
                    thread {[&] () mutable {
                        m_promise.resume();
                    }}.join();
                }
                constexpr void await_resume() const noexcept {}
            };
           
//            return awaitable {*this};
            thread {[&] () mutable {
                resume();
            }}.detach();
            return suspend_always {};

            
        }
        
        auto final_suspend () noexcept {
            
            struct awaitable {
                
                bool await_ready () noexcept {
                    return false;
                }
                
                coroutine_handle<> await_suspend (coroutine_handle <promise_type> thisCoro) noexcept {
                    auto& promise = thisCoro.promise();
                    if (promise.m_continuation) {
                        return static_cast <coroutine_handle<>> (promise.m_continuation);
                    }
                    return noop_coroutine();
                }
                
                void await_resume () noexcept {
                    
                }
            };
//            cout << "final_suspend::" << m_called_from << endl;

            return awaitable {};
        }
        
        auto return_value (auto&& value) {
            m_value = forward <decltype (value)> (value);
        }
        
        auto unhandled_exception () {
            throw runtime_error ("oops");
        }
        
        bool done () {
            return coroutine_handle<promise_type>::from_promise(*this).done();
//            return m_handle.done();
        }
        
        bool resume () {
            if (not coroutine_handle<promise_type>::from_promise(*this).done())
                coroutine_handle<promise_type>::from_promise(*this).resume();
            
            return not coroutine_handle<promise_type>::from_promise(*this).done();
        }
        
    };
    
    bool await_ready () {
        return m_promise.done();
    }
    
    template <typename U>
    auto await_suspend (coroutine_handle <U> continuation) noexcept {
        m_promise.m_continuation = continuation;

//        cout << "me::" << m_promise.m_called_from << endl;
//        cout << "continuation::" << continuation.promise().m_called_from << endl;
//        if constexpr (not U::multithreading) {
//            m_promise.parent_is_not_multithreading_so_join_thread = true;
////            cout << continuation.promise().m_called_from << " is not multithreading!" << endl;
//            return false;
//        } else if (continuation.promise().parent_is_not_multithreading_so_join_thread) {
//            m_promise.parent_is_not_multithreading_so_join_thread = true;
//            return false;
//        }
//        return true;
//        return coroutine_handle<promise_type>::from_promise (m_promise);
    }
    
    auto await_resume () -> decltype (auto) {
        return m_promise.m_value;
    }
    
  
    
    
    promise_type& m_promise;
    
    task (task const&) = delete;
    task (promise_type& promise) : m_promise {promise} {}
    task (task&& o) : m_promise {o.m_promise} {}
    task& operator=(task&&) = delete;
    task& operator=(task const&) = delete;
};
