#pragma once
#include <ph_color/color.hpp>
#include <ph_debug/debug.hpp>
#include <experimental/coroutine>
using namespace std;
using namespace std::experimental;
using namespace std::chrono;

template <class Rep, class Period>
auto operator co_await (duration <Rep, Period> d) {
    
    struct awaitable {
        
        system_clock::duration d_;
        
        bool await_ready () {
            return false;
        }
        
        bool await_suspend (coroutine_handle <> h) {
            cout << "mohaha" << endl;
    
//            this_thread :: sleep_for (1s);
            
            return false;
        }
        
        void await_resume () {
            cout << "resuming" << endl;
        }
        
    };
    
    return awaitable {d};
}


