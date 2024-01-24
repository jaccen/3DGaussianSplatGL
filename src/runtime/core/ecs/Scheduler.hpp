#pragma once

#include <core/ecs/Table.hpp>

#include <typeindex>

namespace jaccen
{
    class Scheduler
    {
    public:
        class Job
        {
            friend class Scheduler;

        public:
            template <typename Resource>
            Job& read();

            template <typename Resource>
            Job& readWrite(TableRef<Resource> resource);

            template <typename Resource>
            Job& readWrite(Resource)
            {
                return *this;
            }

            Job& require(Job&)
            {
                return *this;
            }

            template <typename Function>
            void onExecute(Function)
            {
            }

        private:
            void execute()
            {
            }

        private:
            std::unordered_map<std::type_index, bool> m_readResources;
            std::unordered_map<std::type_index, bool> m_readWriteResources;
        };

    public:
        Job& job()
        {
            m_jobs.emplace_back(std::make_unique<Job>());

            return *m_jobs.back().get();
        }

        void execute()
        {
            for (auto&& job : m_jobs)
            {
                job->execute();
            }

            m_jobs.clear();
        }

    private:
        std::vector<std::unique_ptr<Job>> m_jobs;
    };

    template <typename Resource>
    inline Scheduler::Job& Scheduler::Job::read()
    {
        std::type_index typeId(typeid(Resource));
        assert(!m_readWriteResources.count(typeId) && "Resource already declared as 'readWrite'");
        m_readResources[typeId] = true;
        return *this;
    }

    template <typename Resource>
    inline Scheduler::Job& Scheduler::Job::readWrite(TableRef<Resource> resource)
    {
        std::type_index typeId(typeid(Resource));
        assert(!m_readResources.count(typeId) && "Resource already declared as 'read'");
        m_readWriteResources[typeId] = true;
        return *this;
    }
}