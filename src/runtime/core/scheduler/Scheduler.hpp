#pragma once

#include <core/scheduler/Job.hpp>
#include <core/scheduler/Worker.hpp>

#include <mutex>
#include <queue>
#include <thread>
#include <vector>

namespace wip
{
    class Scheduler
    {
    public:
        static void init(int workerThreads);
        static void stop();

        static void addJob(std::shared_ptr<Job> job);

    private:
        Scheduler() {}

        friend class Worker;
        // Returns true if the worker has received a job or false if it should wait.
        static bool addWorker(std::shared_ptr<Worker> worker);

    private:
        static std::vector<std::shared_ptr<Worker>> m_allWorkers;
        static std::vector<std::unique_ptr<std::thread>> m_threads;

        static std::queue<std::shared_ptr<Job>> m_jobs;
        static std::queue<std::shared_ptr<Worker>> m_freeWorkers;
        static std::mutex m_jobsMutex;
        static std::mutex m_workersMutex;
    };
}