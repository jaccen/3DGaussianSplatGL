#include <core/scheduler/Worker.hpp>

#include <core/Core.hpp>
#include <core/scheduler/Scheduler.hpp>

#include <chrono>
#include <mutex>

using namespace wip;

Worker::Worker()
{
}

void Worker::run()
{
    std::mutex mutex;
    std::unique_lock<std::mutex> lock(mutex);

    while (m_running)
    {
        if (m_ready)
        {
            m_ready = false;
            m_job->process();
            m_job->finish();
        }

        // Tell the scheduler to add us to the free worker pool
        if (Scheduler::addWorker(shared_from_this())) 
        {
            // Use the ready loop to deal with spurious wake-ups 
            while (!m_ready && m_running)
            {
                if (m_cv.wait_for(lock, std::chrono::seconds(1)) == std::cv_status::timeout)
                {
                    // We timed out, but we keep waiting unless
                    // the worker is stopped by the dispatcher
                }
            }
        }
    }
}

void Worker::stop()
{
    m_running = false;
}

void Worker::setWork(std::shared_ptr<Job> job)
{
    m_job = std::move(job);
    m_ready = true;

    m_cv.notify_one();
}

//void Worker::getCondition(std::condition_variable* &cv)
//{
//    cv = &(this->m_cv); // same as &(this)->m_cv in example?
//}
