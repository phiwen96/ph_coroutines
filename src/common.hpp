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
#define USE_NAMESPACES using namespace std; using namespace experimental; using namespace chrono_literals;

// #cmakedefine NAMESPACES @using_namespaces@