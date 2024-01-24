#pragma once

#include <core/ecs/EntityId.hpp>

#include <vector>
#include <bitset>
#include <limits>

namespace jaccen
{
    // TODO: Fixed size sparse set container. Currently not used, because
    // of fixed size requirement, but could be faster than SparseIndex.
    // Test this against SparseIndex i.e. DynamicBitset.
    // TODO: Add iterator
    class SparseSet
    {
    private:
        // TODO: Expose resizability
        static constexpr unsigned k_size = 256;

    public:
        SparseSet()
        {
            m_sparse.resize(k_size, nullptr);
        }

        void insert(EntityId id)
        {
            m_dense.emplace_back(id);
            
            size_t k = static_cast<size_t>(id);

            if (m_sparse.size() <= k)
            {
                m_sparse.resize(k, nullptr);
            }

            m_sparse[k] = &m_dense.back();
        }

        void erase(EntityId id)
        {
            size_t k = static_cast<size_t>(id);

            if (m_sparse.size() <= k)
            {
                return;
            }

            // Swap last value to erased value
            *(m_sparse[k]) = m_dense.back();

            // Update sparse vector
            m_sparse[m_dense.back()] = m_sparse[k];
            m_sparse[k] = nullptr;

            // Remove last value
            m_dense.erase(m_dense.end() - 1);
        }
               
        void clear()
        {
            m_dense.clear();
            m_sparse.clear();
            m_sparse.resize(k_size, nullptr);
        }

        bool check(EntityId id) const
        {
            size_t k = static_cast<size_t>(id);

            if (m_sparse.size() <= k)
            {
                return m_sparse[k] != nullptr;
            }

            return false;
        }

        size_t size() const
        {
            return m_dense.size();
        }

        bool empty() const
        {
            return m_dense.empty();
        }

    private:
        std::vector<EntityId>  m_dense;
        std::vector<EntityId*> m_sparse;
    };
}
