#pragma once

#include <core/ecs/Database.hpp>
#include <core/ecs/Table.hpp>

#include <concepts>

namespace jaccen
{
    template <typename... Tables>
    class Query : public trait::no_copy
    {
    public:
        Query(
            const Database& database, 
            Tables... tables) : 
            m_database(database),
            m_tables(tables...) {}
        Query(Query&&) = default;
        Query& operator=(Query&&) = default;

        // Transform this query filter to include a read-only component table.
        template <typename Component>
        auto hasComponent()
        {
            return newQuery(m_database.table<Component>(), std::index_sequence_for<Tables...>());
        }

        // Transform this query filter to include a mutable component table.
        template <typename Component>
        auto hasComponent(TableRef<Component> table)
        {
            return newQuery(table, std::index_sequence_for<Tables...>());
        }

        // Execute a function for all entities which match the query filter.
        // The function parameters must adhere to the order and constness of
        // the query filter arguments.
        template <typename F>
        void execute(F&& process)
        {
            for (auto&& id : ids())
            {
                processImpl(id, process, std::index_sequence_for<Tables...>());
            }
        }

        // Execute a function for all entities which match the query filter.
        template <typename F>
        void executeIds(F&& process)
        {
            for (auto&& id : ids())
            {
                process(id);
            }
        }

        // Return component data of first entity which matches the query filter,
        // or nullptr if no matches were found.
        template <typename Component>
        const Component* find()
        {
            auto ids = hasComponent<Component>().ids();
            if (!ids.empty())
            {
                return m_database.table<Component>()[ids[0]];
            }
            return nullptr;
        }

        // Return sparse index containing all entity ids which match the query filter.
        SparseIndex index()
        {
            if constexpr (sizeof...(Tables) == 0)
            {
                return SparseIndex();
            }

            auto& table = std::get<0>(m_tables);
            SparseIndex index = table.index();

            forEach(std::index_sequence_for<Tables...>(), m_tables,
                [&](const auto& table)
            {
                index &= table.index();
            });

            return index;
        }

        // Return all entity ids which match the query filter.
        std::vector<EntityId> ids()
        {
            std::vector<EntityId> ids;

            auto idx = index();

            auto it = idx.begin();
            while (it != idx.end())
            {
                ids.emplace_back(*it);
                ++it;
            }

            return ids;
        }

        // TODO: Remove, old implementation for id fetch which is 
        // ~30 times slower than SparseIndex based implementation.
        std::vector<EntityId> idsSlow()
        {
            std::unordered_map<EntityId, unsigned> idCounts;

            forEach(std::index_sequence_for<Tables...>(), m_tables,
                [&](const auto& table)
            {
                // Insert all ids from query tables into map,
                // incrementing occurrence counter as we go
                for (auto& id : table.ids())
                {
                    idCounts[id]++;
                }
            });

            std::vector<EntityId> ids;

            for (auto& kv : idCounts)
            {
                // If id count matches the amount of query tables,
                // the id was located in all tables and is a match
                if (kv.second == sizeof...(Tables))
                {
                    ids.emplace_back(kv.first);
                }
            }

            return ids;
        }

    private:
        template <typename T, size_t... Is>
        auto newQuery(T&& table, std::index_sequence<Is...>)
        {
            return Query<Tables..., decltype(table)>(
                m_database,
                std::get<Is>(m_tables)...,
                table);
        }

        template <size_t... Is, typename Tuple, typename F>
        void forEach(std::index_sequence<Is...>, Tuple&& tuple, F&& f) 
        {
            // Unpack 'tuple' and execute 'f' for each value
            doInOrder([&] { f(std::get<Is>(std::forward<Tuple>(tuple))); }...);
        }

        template <typename... F>
        void doInOrder(F&&... f)
        {
            // Use comma operator to call each function in parameter pack
            int unused[] = { 0, ((void) std::forward<F>(f)(), 0)... };
            (void) unused; // Prevent warning
        }

        template <typename F, size_t... Is>
        void processImpl(EntityId id, F&& f, std::index_sequence<Is...>)
        {
            decltype(auto) getComponent = [](EntityId id, auto& table)
            {
                assert(table[id] != nullptr && "Entity doesn't have component");
                return table[id];
            };

            f(id, *getComponent(id, std::get<Is>(m_tables))...);
        }

    private:
        const Database& m_database;

        std::tuple<Tables...> m_tables;
    };

    // Build a query with read-only access to database.
    Query<> query(const Database& database);
}