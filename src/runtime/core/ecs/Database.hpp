#pragma once

#include <core/Components.hpp>
#include <core/ecs/EntityId.hpp>
#include <core/ecs/Table.hpp>

#include <concepts>

namespace jaccen
{
    // Central storage of the Entity Component System.
    // Stores entities and their components.
    class Database : public trait::no_copy
    {
    public:
        Database();
        Database(Database&&) = default;
        Database& operator=(Database&&) = default;

        template <typename Component>
        TableRef<Component> createTable();

        template <typename Component>
        TableRef<Component> table();

        template <typename Component>
        const TableRef<Component> table() const;

        EntityId createEntity();

        // Remove all Added, Updated, and Deleted components from entities.
        void clearTags();
        // Remove all entities with the Deleted component from the database.
        void purgeDeleted();

    private:
        using TableId = size_t;

        template <typename Component>
        TableId tableId() const;

    private:
        // Container for all tables created from this database.
        std::unordered_map<TableId, std::unique_ptr<ITable>> m_tables;

        TableRef<Added>   m_added;
        TableRef<Updated> m_updated;
        TableRef<Deleted> m_deleted;

        // TODO: consider changing this to 'mutable' to allow 
        // entity creation from const database reference.
        EntityId m_nextEntityId = 1u;
    };

    template <typename Component>
    inline TableRef<Component> Database::createTable()
    {
        static TableId id = tableId<Component>();

        assert(!m_tables.count(id) && "Table already created");

        m_tables[id] = std::make_unique<Table<Component>>();

        return table<Component>();
    }

    template <typename Component>
    inline TableRef<Component> Database::table()
    {
        static TableId id = tableId<Component>();

        assert(m_tables.count(id) && "No such table");

        return static_cast<TableRef<Component>>(*m_tables.at(id));
    }

    template <typename Component>
    inline const TableRef<Component> Database::table() const
    {
        static TableId id = tableId<Component>();

        assert(m_tables.count(id) && "No such table");

        return static_cast<const TableRef<Component>>(*m_tables.at(id));
    }

    template <typename Component>
    inline Database::TableId Database::tableId() const
    {
        return typeid(Component).hash_code();
    }
}
