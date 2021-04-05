#include <experimental/coroutine>
#include <concepts>
#include <ph_debug/debug.hpp>
#include <vector>
#include <thread>

using namespace std;
using namespace experimental;
using namespace chrono_literals;


template <typename T>
struct task {
    
    struct promise_type {
        #define class_name "promise_type"
        promise_type (debug_called_from) : m_called_from {"called_from_"} {
            m_called_from += _called_from_function;
            m_called_from += "::";
            m_called_from += to_string (_called_from_line);
        }
        friend ostream& operator<< (ostream& os, promise_type const& p) {
            os << cyan << p.m_called_from << white;
            return os;
        }
        string m_called_from;
        coroutine_handle <> m_continuation;
        T m_value;
        
        auto get_return_object () {
            return task {*this};
        }
        
        /**
         if parent is not detaching:
            t1:
         */
        auto initial_suspend () {
//            cout << "initial_suspend::" << m_called_from << endl;
            struct awaitable {
                promise_type& m_promise;
                bool await_ready() const noexcept {
                    cout << "\t" << green << "initial suspend::" << m_promise << endl;
                    return false;
                }
                bool await_suspend(coroutine_handle <>) const noexcept {
                    thread {[&] () mutable {
                        m_promise.resume();
                    }}.detach();
//                    threadss.emplace_back([&] () mutable {
//                        m_promise.resume();
//                    });
                    return true;
                }
                constexpr void await_resume() const noexcept {}
            };
           
            return awaitable {*this};
        }
        
        auto final_suspend () noexcept {
            
            struct awaitable {
                promise_type& m_promise;
                bool await_ready () noexcept {
                    cout << "\t" << blue << "final suspend::" << m_promise << endl;
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

            return awaitable {*this};
        }
        
        auto return_value (auto&& value) {
            m_value = forward <decltype (value)> (value);
        }
        
        auto unhandled_exception () {
            throw runtime_error ("oops");
        }
        
        bool done () {
            return coroutine_handle<promise_type>::from_promise(*this).done();
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
    bool await_suspend (coroutine_handle <U> continuation) noexcept {
        cout << "\t" << red << "co_await" << white <<  " me:" << m_promise << " parent:" << continuation.promise() << endl;
        m_promise.m_continuation = continuation;
        return true;
    }
    
    auto await_resume () -> decltype (auto) {
        return m_promise.m_value;
    }
    
  
    
    
    promise_type& m_promise;
    
    void wait () {
        while (not m_promise.done()) {
            
        }
    }
    
    task (task const&) = delete;
    task (promise_type& promise) : m_promise {promise} {}
    task (task&& o) : m_promise {o.m_promise} {}
    task& operator=(task&&) = delete;
    task& operator=(task const&) = delete;
};
