#if !defined TIMER_HPP
#define TIMER_HPP

#include <time.h>

struct Timer {
    time_t begin;
    Timer()
        : begin(0)
    { }

    void
    start() {
        begin = time(NULL);
    }

    time_t
    stop() {
        time_t now = time(NULL);
        time_t diff = now - begin;
        return diff;
    }
};

#endif // TIMER_HPP
