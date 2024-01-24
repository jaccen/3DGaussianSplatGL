#include <core/scheduler/Scheduler.hpp>

#include <core/Core.hpp>
#include <core/scheduler/Worker.hpp>

#include <iostream>

using namespace wip;

std::vector<std::shared_ptr<Worker>> Scheduler::m_allWorkers;
std::vector<std::unique_ptr<std::thread>> Scheduler::m_threads;
std::queue<std::shared_ptr<Job>> Scheduler::m_jobs;
std::queue<std::shared_ptr<Worker>> Scheduler::m_freeWorkers;
std::mutex Scheduler::m_jobsMutex;
std::mutex Scheduler::m_workersMutex;

void Scheduler::init(int workerThreads)
{
    for (int i = 0; i < workerThreads; ++i)
    {
        auto w = std::make_shared<Worker>();
        m_allWorkers.emplace_back(w);

        auto t = std::make_unique<std::thread>(&Worker::run, std::move(w));
        m_threads.emplace_back(std::move(t));
    }
}

void Scheduler::stop()
{
    for (auto& w : m_allWorkers)
    {
        w->stop();
    }
    std::cout << "Stopped workers.\n";

    for (auto& t : m_threads)
    {
        t->join();
    }
    std::cout << "Joined threads.\n";
}

void Scheduler::addJob(std::shared_ptr<Job> job)
{
    m_workersMutex.lock();

    // Assign the job to a free worker, if any available,
    // or add the job to the work queue
    if (!m_freeWorkers.empty()) 
    {
        m_freeWorkers.front()->setWork(std::move(job));

        //std::condition_variable* cv;
        //worker->getCondition(cv);
        //cv->notify_one();

        m_freeWorkers.pop();
        m_workersMutex.unlock();
    }
    else
    {
        m_workersMutex.unlock();

        m_jobsMutex.lock();
        m_jobs.emplace(std::move(job));
        m_jobsMutex.unlock();
    }
}

bool Scheduler::addWorker(std::shared_ptr<Worker> worker)
{
    m_jobsMutex.lock();

    // Assign a pending job to the worker, if any available, 
    // or add the worker to the worker queue
    if (!m_jobs.empty())
    {
        worker->setWork(std::move(m_jobs.front()));

        m_jobs.pop();
        m_jobsMutex.unlock();

        return false; // Has received a job
    }
    else
    {
        m_jobsMutex.unlock();

        m_workersMutex.lock();
        m_freeWorkers.emplace(std::move(worker));
        m_workersMutex.unlock();

        return true; // Should wait for a job
    }
}