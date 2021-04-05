#include <experimental/coroutine>
#include <concepts>
#include <ph_debug/debug.hpp>


using namespace std;
using namespace experimental;
using namespace chrono_literals;


atomic<int> ccount {0};

template <typename T, bool _multithreading = true, bool continuation_multithreading = true>
struct task {
    
    struct promise_type {
        #define class_name "promise_type"
        vector <thread> m_threads;
        inline static constexpr bool multithreading = _multithreading;
        promise_type (debug_called_from) : m_called_from {"called_from_"} {
            m_called_from += _called_from_function;
//            debug_class_print_called_from(green, 0)
        }
        friend ostream& operator<< (ostream& os, promise_type const& p) {
            os << cyan << p.m_called_from << white;
            return os;
        }
        string m_called_from;
        int m_ccount = ccount++;
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

//                    if (not continuation_multithreading)
//                        return true;
                    if constexpr (multithreading)
                    {
                        if (not continuation_multithreading)
                        {
                            cout << "lol" << endl;
                            
//                            return true;
                            return false;
                        }
//                        cout << m_promise.m_called_from << " oja" << endl;
                        return false;
                        
                    } else
                    {
//                        cout << m_promise.m_called_from << " onej" << endl;

                        return true;
                    }
                    return false;
                }
                bool await_suspend(coroutine_handle <>) const noexcept {
//                    cout << "initial suspend::" << m_promise.m_called_from << endl;

                    if constexpr (multithreading)
                    {
                        if constexpr (not continuation_multithreading)
                        {
                            thread {[&] () mutable {
                                m_promise.resume();
                            }}.join();
                            return true;
                        }
                        else
                        {
                            thread {[&] () mutable {
                                m_promise.resume();
                            }}.detach();
                            return true;
                        }
                    }
                    // WORKS
                    else {
                        cout << "whaaat" << endl;
                        m_promise.resume ();
                        return true;
                    }
                    
                }
                constexpr void await_resume() const noexcept {}
            };
           
            return awaitable {*this};
//            thread {[&] () mutable {
//                resume();
//            }}.detach();
//            return suspend_always {};

            
        }
        
        auto final_suspend () noexcept {
            
            struct awaitable {
                promise_type& m_promise;
                bool await_ready () noexcept {
                    cout << "\t" << blue << "final suspend::" << m_promise << endl;

                    if (not continuation_multithreading)
                    {
//                        cout << "lol" << endl;
                        return false;
                    }
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
//            return m_handle.done();
        }
        
        bool resume () {
            if (not coroutine_handle<promise_type>::from_promise(*this).done())
                coroutine_handle<promise_type>::from_promise(*this).resume();
            
            return not coroutine_handle<promise_type>::from_promise(*this).done();
        }
        
    };
    
    bool await_ready () {
        if constexpr (not _multithreading) {
            cout << "lol" << endl;
        }
        if (not continuation_multithreading)
        {
            cout << "lol" << endl;
            return true;
        }
        return m_promise.done();
    }
    
    template <typename U>
    bool await_suspend (coroutine_handle <U> continuation) noexcept {
        if constexpr (not U::multithreading) {
//            cout << continuation.promise().m_called_from << " is not multithreading!" << endl;
//            m_promise.continuation_multithreading = false;
        }
//        cout << m_promise.m_called_from << "::" << continuation.promise().m_called_from << endl;
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
//        if constexpr (_multithreading)
//            return true;
//        else
//            return false;
//        return true;
        return true;
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
