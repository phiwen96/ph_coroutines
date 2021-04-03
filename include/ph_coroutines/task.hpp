#include <experimental/coroutine>
#include <concepts>


using namespace std;
using namespace experimental;
using namespace chrono_literals;



template <typename T, bool multithreading = true>
struct task {
    
    struct promise_type {
        
        coroutine_handle <> m_continuation;
        T m_value;
        
        auto get_return_object () {
            return task {*this};
        }
        
        auto initial_suspend () {
            if constexpr (multithreading) {
                thread {[&] () mutable {
                    resume();
                }}.detach();
                return suspend_always {};
            } else {
                return suspend_never {};
            }
            cout << "yo" << endl;
            
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
    
    auto operator co_await () & {
        
        struct awaitable {
            
            promise_type& m_promise;
            
            bool await_ready () {
                return m_promise.done();
            }
            
            auto await_suspend (coroutine_handle <> continuation) noexcept {
                m_promise.m_continuation = continuation;
                return true;
        //        return coroutine_handle<promise_type>::from_promise (m_promise);
            }
            
            auto await_resume () -> decltype (auto) {
                return m_promise.m_value;
            }
        };
        
        return awaitable {m_promise};
    }
    
    auto operator co_await () && {
        struct awaitable {
            
            promise_type& m_promise;
            
            bool await_ready () {
                return true;
            }
            
            auto await_suspend (coroutine_handle <> continuation) noexcept {
                m_promise.m_continuation = continuation;
                return true;
//                return coroutine_handle<promise_type>::from_promise (m_promise);
            }
            
            auto await_resume () -> decltype (auto) {
                return m_promise.m_value;
            }
        };
        
        return awaitable {m_promise};
    }
    
    
    promise_type& m_promise;
    
    task (task const&) = delete;
    task (promise_type& promise) : m_promise {promise} {}
    task (task&& o) : m_promise {o.m_promise} {}
    task& operator=(task&&) = delete;
    task& operator=(task const&) = delete;
};
