#pragma once

#include <pch.hpp>

#include <cu/scheduler/job.hpp>
#include <cu/scheduler/ctpl_stl.hpp>
#include <cu/scheduler/interruptable_sleep.hpp>




// Based on: https://github.com/Bosma/Scheduler/blob/master/Scheduler.h

namespace cu {

    inline bool try_parse(std::tm& tm, const std::string& expression, const std::string& format) {
        std::stringstream ss(expression);
        return !(ss >> std::get_time(&tm, format.c_str())).fail();
    }

    class scheduler
    {
        using scheduler_clock = std::chrono::system_clock;
    public:
        explicit scheduler(const std::uint8_t& max_jobs = 10)
            : m_done(false), m_threads(max_jobs + 1)
        {
            m_threads.push([this](int) {
                spdlog::info("Starting scheduler thread.");
                while (!m_done) {
                    if (m_jobs.empty()) {
                        m_sleeper.sleep();
                    }
                    else {
                        auto time_of_first_task = (*m_jobs.begin()).first;
                        m_sleeper.sleep_until(time_of_first_task);
                    }
                    manage_tasks();
                }
                spdlog::info("Scheduler thread exiting.");
                });
        }

        scheduler(const scheduler&) = delete;
        scheduler(scheduler&&) noexcept = delete;
        scheduler& operator=(const scheduler&) = delete;
        scheduler& operator=(scheduler&&) noexcept = delete;

        ~scheduler()
        {
            spdlog::info("Shutting down scheduler.");
            m_done = true;
            m_sleeper.interrupt();
        }
        
        template<typename _Callable, typename... _Args>
        void every(const scheduler_clock::duration& duration, const std::string& label, 
            _Callable&& function, _Args &&... args)
        {
            const auto ajob = std::make_shared<every_job>(label, duration, 
                std::bind(std::forward<_Callable>(function), std::forward<_Args>(args)...));
            const auto next_time = ajob->next_run_time();
            add_job(next_time, std::move(ajob));
        }

        template<typename _Callable, typename... _Args>
        void in(const scheduler_clock::time_point time, _Callable&& f, _Args &&... args) 
        {
            const auto ajob = std::make_shared<in_job>(
                std::bind(std::forward<_Callable>(f), std::forward<_Args>(args)...));
            add_job(time, std::move(ajob));
        }

        template<typename _Callable, typename... _Args>
        void in(const scheduler_clock::duration time, _Callable&& f, _Args &&... args) 
        {
            in(scheduler_clock::now() + time, std::forward<_Callable>(f), std::forward<_Args>(args)...);
        }

        template<typename _Callable, typename... _Args>
        void at(const std::string& time, _Callable&& f, _Args &&... args) 
        {
            // get current time as a tm object
            const auto time_now = scheduler_clock::to_time_t(scheduler_clock::now());
            //std::tm tm = *std::localtime(&time_now);
            auto tm = *std::localtime(&time_now);

            // our final time as a time_point
            scheduler_clock::time_point tp;

            if (try_parse(tm, time, "%H:%M:%S")) {
                // convert tm back to time_t, then to a time_point and assign to final
                tp = scheduler_clock::from_time_t(std::mktime(&tm));

                // if we've already passed this time, the user will mean next day, so add a day.
                if (scheduler_clock::now() >= tp)
                    tp += std::chrono::hours(24);
            }
            else if (try_parse(tm, time, "%Y-%m-%d %H:%M:%S")) {
                tp = scheduler_clock::from_time_t(std::mktime(&tm));
            }
            else if (try_parse(tm, time, "%Y/%m/%d %H:%M:%S")) {
                tp = scheduler_clock::from_time_t(std::mktime(&tm));
            }
            else {
                // could not parse time
                throw std::runtime_error("Cannot parse time string: " + time);
            }

            in(tp, std::forward<_Callable>(f), std::forward<_Args>(args)...);
        }

        // from https://en.wikipedia.org/wiki/Cron#Overview

        template<typename _Callable, typename... _Args>
        void cron(const std::string& expression, const std::string& label, 
            _Callable&& function, _Args &&... args) 
        {
            const auto ajob = std::make_shared<cron_job>(label, expression,
                std::bind(std::forward<_Callable>(function), std::forward<_Args>(args)...));
            const auto next_time = ajob->next_run_time();
            add_job(next_time, std::move(ajob));
        }

        template<typename _Callable, typename... _Args>
        void interval(const scheduler_clock::duration time, _Callable&& f, _Args &&... args) 
        {
            const auto ajob = std::make_shared<every_job>(time, std::bind(std::forward<_Callable>(f),
                std::forward<_Args>(args)...), true);
            add_job(scheduler_clock::now(), std::move(ajob));
        }
    private:
        std::atomic<bool> m_done;
        interruptable_sleep m_sleeper;
        std::multimap<scheduler_clock::time_point, std::shared_ptr<job>> m_jobs;
        std::mutex m_lock;
        ctpl::thread_pool m_threads;

        void add_job(const scheduler_clock::time_point time, std::shared_ptr<job> ajob)
        {
            std::lock_guard<std::mutex> l(m_lock);
            m_jobs.emplace(time, std::move(ajob));
            m_sleeper.interrupt();
        }

        void manage_tasks() 
        {
            std::lock_guard<std::mutex> l(m_lock);

            const auto end_of_tasks_to_run = m_jobs.upper_bound(scheduler_clock::now());

            // if there are any tasks to be run and removed
            if (end_of_tasks_to_run != m_jobs.begin()) 
            {
                // keep track of tasks that will be re-added
                decltype(m_jobs) recurred_tasks;

                // for all tasks that have been triggered
                for (auto i = m_jobs.begin(); i != end_of_tasks_to_run; ++i) 
                {
                    auto& ajob = (*i).second;

                    if (ajob->interval()) {
                        // if it's an interval task, only add the task back after f() is completed
                        m_threads.push([this, ajob](int) {
                            ajob->run();
                            // no risk of race-condition,
                            // add_task() will wait for manage_tasks() to release lock
                            add_job(ajob->next_run_time(), ajob);
                            });
                    }
                    else {
                        m_threads.push([ajob](int) {
                            ajob->run();
                            });
                        // calculate time of next run and add the new task to the tasks to be recurred
                        if (ajob->recurring())
                            recurred_tasks.emplace(ajob->next_run_time(), std::move(ajob));
                    }
                }

                // remove the completed tasks
                m_jobs.erase(m_jobs.begin(), end_of_tasks_to_run);

                // re-add the tasks that are recurring
                for (auto& task : recurred_tasks)
                {
                    m_jobs.emplace(task.first, std::move(task.second));
                }
            }
        }
    };
}