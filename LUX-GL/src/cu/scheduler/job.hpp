#pragma once

#include <pch.hpp>

#include <croncpp/croncpp.h>


namespace cu {

    class job
    {
        using scheduler_clock = std::chrono::system_clock;

    public:
        job(const std::string& label, std::function<void()>&& function, 
            bool recurring = false,
            bool interval = false)
            : m_label(label), m_function(std::move(function)), m_recurring(recurring), m_interval(interval)
        {}
        ~job() = default;

        const std::string label() const { return m_label; }
        const bool recurring() const { return m_recurring; }
        const bool interval() const { return m_interval; }

        virtual const scheduler_clock::time_point next_run_time() const = 0;

        void run()
        {
            try {
                m_function();
            }
            catch (const std::exception& exception) {
                spdlog::critical("Job [{}] Error: {}", label(), exception.what());
            }
        }
    private:
        std::string m_label;
        std::function<void()> m_function;
        bool m_recurring;
        bool m_interval;
    };

    class every_job : public job
    {
    public:
        every_job(const std::string& label,           
            const scheduler_clock::duration& duration,
            std::function<void()>&& function)
            : job(label, std::move(function), true), m_duration(duration)
        {}

        const scheduler_clock::duration duration() const { return m_duration; }

        const scheduler_clock::time_point next_run_time() const override {
            return scheduler_clock::now() + m_duration;
        }
    private:
        scheduler_clock::duration m_duration;
    };


    class cron_job : public job {
    public:
        cron_job(const std::string& label, 
            const std::string& expression, 
            std::function<void()>&& function) 
            : job(label, std::move(function), true), cron_expression(expression) {}

        const scheduler_clock::time_point next_run_time() const override {
            const auto time_now = scheduler_clock::to_time_t(scheduler_clock::now());
            const auto cron = cron::make_cron(cron_expression);
            const auto some_time = cron::cron_next(cron, time_now);
            const auto next_time = scheduler_clock::from_time_t(some_time);
            return next_time;
        };
    private:
        std::string cron_expression;        
    };


    class in_job : public job {
    public:
        explicit in_job(const std::string& label, std::function<void()>&& function) 
            : job(label, std::move(function)) {}

        // dummy time_point because it's not used
        const scheduler_clock::time_point next_run_time() const override {
            return scheduler_clock::time_point(scheduler_clock::duration(0)); 
        }
    };
}