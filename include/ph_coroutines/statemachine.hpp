#pragma once
#include <experimental/coroutine>
#include <concepts>
#include <ph_debug/debug.hpp>
#include <vector>
#include <thread>
#include <any>
#include <tuple>
#include <typeinfo>

using namespace std;
using namespace experimental;
using namespace chrono_literals;



template <typename... Events>
struct Event
{
    
};


struct StateMachine
{
    struct promise_type
    {
        #define class_name "StateMachine::promise_type"
        using CoroHandle = coroutine_handle <promise_type>;
        
        auto get_return_object () noexcept -> decltype (auto)
        {
            return StateMachine {CoroHandle::from_promise (*this)};
        }
        
        auto initial_suspend () const noexcept -> decltype (auto)
        {
            return suspend_never {};
        }
        
        auto final_suspend () const noexcept -> decltype (auto)
        {
            return suspend_always {};
        }
        
        auto return_void () noexcept -> void
        {
            
        }
        
        auto unhandled_exception () noexcept -> void
        {
            
        }
        
        std::any currentEvent;
        
        bool (*isWantedEvent) (std::type_info const&) = nullptr;
        
        template <typename... E>
        auto await_transform (Event <E...>, d0) -> decltype (auto)
        {
            d1 (red, 0)

            cout << "hi" << endl;
            
            isWantedEvent = [] (type_info const& type) -> bool
            {
                return ((type == typeid (E)) || ...);
            };
            
            struct awaitable
            {
                #define class_name "StateMachine::promise_type::awaitable"
                
                auto await_ready (d0) -> bool
                {
                    d1 (yellow, 0)
                    return false;
                }
                
                auto await_suspend (coroutine_handle <promise_type> c, d0) -> void
                {
                    d1 (yellow, 0)

                }
                
                auto await_resume (d0) -> decltype (auto)
                {
                    d1 (yellow, 0)

                    variant <E...> event;
//                    (void) ((currentEvent -> type () == typeid (E) ? (event = move (*any_cast <E> (currentEvent)), true) : false) || ...);
                    return event;
                }
                
                const any * currentEvent;
            };
            
            return awaitable {&currentEvent};
        }
    };
    #define class_name "StateMachine"

    template<typename E>
    auto onEvent (E e, d0) -> void
    {
        d1 (green, 0)

        auto& promise = coro.promise ();
        
        if (promise.isWantedEvent (typeid (E)))
        {
            cout << "OK" << endl;
            promise.currentEvent = forward <E> (e);
            coro ();
        }
        else
        {
            cout << "NOT OK" << endl;
        }
    }
    
    ~StateMachine ()
    {
      coro.destroy();
    }
    
    StateMachine (StateMachine&& other) : coro {exchange (other.coro, {})}
    {
        
    }
    
    StateMachine (StateMachine const&) = delete;
    StateMachine &operator= (StateMachine const&) = delete;
private:
    StateMachine (coroutine_handle <promise_type> coro) : coro {coro}
    {
        
    }
    
    coroutine_handle <promise_type> coro;
};




















struct Any {};




template <class Promise>
struct Awaitab
{
    Promise& m_promise;
    static inline int i = 0;
    int m_i = i++;

    Awaitab (Promise& p, string const& t) : m_promise {p}
    {
        //            isWanted = [&](char cc){return ((cc == c) || ...);};
        m_promise.isWanted = [&] (char cc)
        {
            for (auto const& c : t)
            {
                if (cc == c)
                    return true;
            }
            return false;
        };
    }
    
    Awaitab (Promise& p, Any const& t) : m_promise {p}
    {
        //            isWanted = [&](char cc){return ((cc == c) || ...);};
        m_promise.isWanted = [&](char cc){return true;};
    }
    
    auto await_ready ()
    {
        return false;
    }
    auto await_suspend (coroutine_handle <Promise> continuation)
    {
        m_promise.m_continuation = continuation;
        return true;
//        cout << "yo::" << m_i << endl;
//        continuation.resume();
//        return continuation;
    }
    auto await_resume ()
    {
        return m_promise.current_char;
    }
};

//template <class Promise, class T>
//struct Awaitab <Promise, T> : Awaitab <Promise>
//{
//    using Awaitab<Promise>::m_promise;
//    Awaitab (Promise& p, T const& t) : Awaitab<Promise> {p}
//    {
//        //            isWanted = [&](char cc){return ((cc == c) || ...);};
//        m_promise.isWanted = [&](char cc){for(auto const& c : t){if (cc == c)return true;}return false;};
//    }
//
//};
//

//template <class Promise>
//template <>
//Awaitab<Promise>::Awaitab<Any>(Promise&, Any const&) {
//
//}

template <typename promise_type>
struct final_awaitable
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


struct machine
{
    struct promise_type : coroutine_handle <promise_type>
    {
        int m_value {0};
        
        promise_type () : coroutine_handle <promise_type> {coroutine_handle <promise_type>::from_promise(*this)}
        {
            
        }
        auto initial_suspend ()
        {
            return suspend_never {};
        }
        auto final_suspend () noexcept
        {
            return final_awaitable <promise_type> {*this};
        }
        auto unhandled_exception ()
        {
            
        }
        auto get_return_object ()
        {
            return machine {*this};
        }
        auto return_void ()
        {
            
        }
        auto yield_value (int value)
        {
            m_value += value;
            cout << m_value << endl;
            return suspend_never {};
        }
        
        
        
        auto await_transform (suspend_always const& t)
        {
            return t;
        }
        auto await_transform (suspend_never const& t)
        {
            return t;
        }
        
//        auto await_transform (string const& t) -> decltype (auto)
//        {
//            return Awaitab <promise_type> {*this, t};
//        }

        auto await_transform (machine const& m)
        {
            struct awaitable
            {
                promise_type& m_promise;

                auto await_ready () noexcept -> bool
                {
//                    return true;
                    return coroutine_handle <promise_type>::from_promise (m_promise).done ();
                }

                auto await_suspend (coroutine_handle <void> co_awaiting) noexcept -> bool
                {
                    m_promise.m_continuation = co_awaiting;
//                    continuation.resume();
//                    return true;
                    return true;
                }

                auto await_resume () noexcept -> decltype (auto)
                {
                    
//                    return m_promise.m_value;
                }
            };

            return awaitable {*this};
        }
        
        auto await_transform (string const& t) -> decltype (auto)
        {
            return Awaitab <promise_type> {*this, t};
        }
        
        auto await_transform (Any const& t) -> decltype (auto)
        {
            return Awaitab <promise_type> {*this, t};
        }
        
//        auto await_transform () -> decltype (auto)
//        {
//            return Awaitab <promise_type> {*this};
//        }
        
        
        function<bool(char)> isWanted;
        char current_char;
        coroutine_handle <> m_continuation;
        

    };
    

    operator int ()
    {
        return m_promise.m_value;
    }
    void process (char c)
    {
        if (m_promise.isWanted (c))
        {
            m_promise.current_char = c;
            m_promise ();
        }
        else
        {
            
        }
    }
    
    promise_type& m_promise;
};
