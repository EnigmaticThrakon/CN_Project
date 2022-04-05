#ifndef TIMER_H
#define TIMER_H

#include <ctime>
#include <chrono>

class update_timer
{
private:
    unsigned long nanoseconds;
public:
    update_timer() { nanoseconds = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count(); }
    ~update_timer() { }
    unsigned long elapsed_time() { return std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count() - nanoseconds; }
    void reset() { nanoseconds = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count(); }

};

#endif