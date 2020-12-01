/**
 * Macros to calculate easily how much time does a particular procedure take to execute
 * add -DBENCHMARKS to include it
 */
#ifdef BENCHMARKS
    #include <chrono>
    #include <iostream>

    #define _TIME_POINT          std::chrono::steady_clock::time_point
    #define _CLOCK_NOW_          std::chrono::steady_clock::now
    #define _DURATION_MILlISECS_ std::chrono::duration_cast<std::chrono::milliseconds>

    #define BEGIN_TIME _TIME_POINT _chrono_begin_ = _CLOCK_NOW_()
    #define END_TIME(x)                                                                        \
        _TIME_POINT _chrono_end_ = _CLOCK_NOW_();                                              \
        std::cout << x << " : " << _DURATION_MILlISECS_(_chrono_end_ - _chrono_begin_).count() \
                  << "[ms]" << std::endl
    #define SIZEOFCONTAINER(msg, sz) std::cout << "size of " << msg << ": " << sz << std::endl;
#else
    #define BEGIN_TIME \
        {}
    #define END_TIME(x) \
        {}
    #define SIZEOFCONTAINER(msg, sz) \
        {}
#endif
