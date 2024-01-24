#pragma once

#include <core/Traits.hpp>
#include <core/scheduler/Job.hpp>

#include <condition_variable>

namespace wip
{
    // Worker represents a worker thread for the job scheduler.
    class Worker : public trait::no_copy, public std::enable_shared_from_this<Worker>
    {
    public:
        Worker();

        void run();
        void stop();

        void setWork(std::shared_ptr<Job> job);

    private:
        bool m_running = true;
        bool m_ready = false;

        std::shared_ptr<Job> m_job;
        std::condition_variable m_cv;
    };
}