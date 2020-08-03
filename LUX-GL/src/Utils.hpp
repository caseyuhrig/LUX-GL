#pragma once

//#include <time.h>
#include <ctime>


namespace Utils {

    

    class Time
    {
    public:
        struct HMS
        {
            int Hour, Minute, Second;
        };

        // TODO Remove once C++20 is ready and use the new chrono updates.
        static HMS now()
        {
            // time in seconds 
            time_t s = time(NULL);
            // to get current time 
            struct tm*  current_time = localtime(&s);
            return { current_time->tm_hour, current_time->tm_min, current_time->tm_sec };
        }
    };
}

