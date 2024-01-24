#include <core/ecs/Database.hpp>

using namespace jaccen;

Database::Database() :
    m_added(createTable<Added>()),
    m_updated(createTable<Updated>()),
    m_deleted(createTable<Deleted>())
{
}

void Database::clearTags()
{
    m_added.clear();
    m_updated.clear();
    m_deleted.clear();
}

void Database::purgeDeleted()
{
    for (auto&& id : m_deleted.ids())
    { 
        for (auto& kv : m_tables)
        {
            kv.second->remove(id);
        }
    }
}

EntityId Database::createEntity()
{
    auto id = m_nextEntityId;
    m_nextEntityId++;
    return id;
}
