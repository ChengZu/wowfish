/*
 * Timer.h
 *
 *  Created on: 2016-04-03
 *      Author: ChengZu  Email: 1351606745@qq.com
 */

#ifndef UTILS_TIMER_H_
#define UTILS_TIMER_H_



#ifdef WIN32   // Windows system specific
#include <windows.h>
#else          // Unix based system specific
#include <sys/time.h>
#endif

class Timer
{
public:
    Timer();                                  // default constructor
    ~Timer();                                 // default destructor

    void start();                             // start timer
    void stop();                              // stop the timer

    float getElapsedTime();                  // get elapsed time
    float getElapsedTimeInSec(); 			 // get elapsed time in second (same as getElapsedTime)
    float getElapsedTimeInMilliSec();        // get elapsed time in milli-second
    float getElapsedTimeInMicroSec();        // get elapsed time in micro-second

protected:

private:
    float startTimeInMicroSec;                 // starting time in micro-second
    float endTimeInMicroSec;                   // ending time in micro-second
    int stopped;                               // stop flag
#ifdef WIN32
    LARGE_INTEGER frequency;                   // ticks per second
    LARGE_INTEGER startCount;                  //
    LARGE_INTEGER endCount;                    //
#else
    timeval startCount;                        //
    timeval endCount;//
#endif
};



#endif /* UTILS_TIMER_H_ */
