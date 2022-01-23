#pragma once

#ifndef __COUNTER__
#include "magic/adl_counter.h"
using ADLcounter = constexpr_counter<struct TagA, int, 0, 1>;
#define __COUNTER__ ADLcounter::next()
#endif

#include <exception>

#define COSTART(state)                       \
    constexpr int counterInit = __COUNTER__; \
    auto& COSTATE = state;                   \
    switch(state){                           \
        case 0: {

#define COYIELD(val)                         \
            ++COSTATE;                       \
            return val;                      \
        }                                    \
        case __COUNTER__ - counterInit:{

#define COFINAL(val)                         \
            COSTATE = 0;                     \
            return val;                      \
        }                                    \
        default: {                           \
            throw std::runtime_error(        \
                "Invalid coroutine state");  \
        }                                    \
    }
