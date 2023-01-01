#pragma once

#include <chrono>
#include <thread>
#include <future>
#include <mutex>
#include <sstream>

// https://github.com/Bosma/Scheduler/blob/master/InterruptableSleep.h

namespace cu {

    /* 
    * interruptable_sleep offers a sleep that can be interrupted by any thread.
    * It can be interrupted multiple times
    * and be interrupted before any sleep is called (the sleep will immediately complete)
    * Has same interface as condition_variables and futures, except with sleep instead of wait.
    * For a given object, sleep can be called on multiple threads safely, but is not recommended as behaviour is undefined.
    */
    class interruptable_sleep 
    {
        using Clock = std::chrono::system_clock;

    public:
        interruptable_sleep() : interrupted(false) {
        }

        interruptable_sleep(const interruptable_sleep&) = delete;

        interruptable_sleep(interruptable_sleep&&) noexcept = delete;

        ~interruptable_sleep() noexcept = default;

        interruptable_sleep& operator=(const interruptable_sleep&) noexcept = delete;

        interruptable_sleep& operator=(interruptable_sleep&&) noexcept = delete;

        void sleep_for(Clock::duration duration) {
            std::unique_lock<std::mutex> ul(m);
            cv.wait_for(ul, duration, [this] { return interrupted; });
            interrupted = false;
        }

        void sleep_until(Clock::time_point time) {
            std::unique_lock<std::mutex> ul(m);
            cv.wait_until(ul, time, [this] { return interrupted; });
            interrupted = false;
        }

        void sleep() {
            std::unique_lock<std::mutex> ul(m);
            cv.wait(ul, [this] { return interrupted; });
            interrupted = false;
        }

        void interrupt() {
            std::lock_guard<std::mutex> lg(m);
            interrupted = true;
            cv.notify_one();
        }

    private:
        bool interrupted;
        std::mutex m;
        std::condition_variable cv;
    };
}