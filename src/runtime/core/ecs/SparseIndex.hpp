#pragma once

#include <core/ecs/EntityId.hpp>

#include <vector>
#include <bitset>
#include <limits>

namespace jaccen
{
    // TODO: Rename to DynamicBitset?
    class SparseIndex
    {
    public:
        void insert(EntityId id);
        void erase(EntityId id);
        void clear();

        bool check(EntityId id) const;

        size_t size() const;
        bool empty() const;

        SparseIndex& operator|=(const SparseIndex& other);
        SparseIndex& operator&=(const SparseIndex& other);
        SparseIndex& operator^=(const SparseIndex& other);

        friend SparseIndex operator|(const SparseIndex& lhs, const SparseIndex& rhs);
        friend SparseIndex operator&(const SparseIndex& lhs, const SparseIndex& rhs);
        friend SparseIndex operator^(const SparseIndex& lhs, const SparseIndex& rhs);

    public:
        class Iterator
        {
        public:
            using iterator_category = std::input_iterator_tag;
            using value_type = EntityId;
            using difference_type = EntityId;
            using pointer = const EntityId*;
            using reference = EntityId&;

            explicit Iterator(
                const SparseIndex& container,
                size_t size,
                size_t pos) :
                m_container(container),
                m_size(size),
                m_pos(pos)
            {}

            Iterator& operator++();
            Iterator operator++(int);

            bool operator==(Iterator other) const;
            bool operator!=(Iterator other) const;
            
            value_type operator*() const;

        private:
            const SparseIndex& m_container;
            // Total number of bits in iterated container.
            size_t m_size;
            // Current bit position of iterator.
            size_t m_pos;
        };

    public:
        Iterator begin() const;
        Iterator end() const;

    private:
        struct Position
        {
            // Index to vector entry where the bit of the entity is stored
            size_t index;
            // Bit within the indexed entry which marks whether the entity exists or not
            unsigned bit;
        };

        static Position bitPos(uint32_t pos);
        
        void allocateBlock();

    private:
        static constexpr unsigned k_bitsPerBlock = 64;
        using DataBlock = std::bitset<k_bitsPerBlock>;

        std::vector<DataBlock> m_bits;
    };

    SparseIndex operator|(const SparseIndex& lhs, const SparseIndex& rhs);
    SparseIndex operator&(const SparseIndex& lhs, const SparseIndex& rhs);
    SparseIndex operator^(const SparseIndex& lhs, const SparseIndex& rhs);
}
