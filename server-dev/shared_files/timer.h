#ifndef TIMER_H
#define TIMER_H

#include <ctime>
#include <chrono>

/*
 * Simple timer class
 *      Utilizes the ctime and chrono libraries to hold the count of nanoseconds since the Epoch
 *      and returns the count between two different time locations: the point when the timer is 
 *      reset and when the elapsed_time functions are called
 * 
 *      Allows for simple nanosecond timing
 */

class update_timer
{
private:
    //Private variable that is set when the user calls the reset() function
    unsigned long nanoseconds;

public:
    //Constructor that sets the nanosecond counter variable
    update_timer() 
    { 
        nanoseconds = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count(); 
    }

    //Default deconstructor
    ~update_timer() { }

    //Function to return the count of nanoseconds that have passed between the time this function is called and when the reset
    //      function was called
    //Returns an unsigned long to help prevent overflows with the amount of nanoseconds that have passed
    unsigned long elapsed_time() 
    { 
        return std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count() - nanoseconds; 
    }

    //Function to set the nanosecond counter variable at a specific location
    void reset() 
    { 
        nanoseconds = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count(); 
    }
};

#endif