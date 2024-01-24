#include <core/ecs/SparseIndex.hpp>

#include <core/Core.hpp>

using namespace jaccen;

namespace
{
    size_t skipEmptyBits(
        const SparseIndex& index,
        size_t from,
        size_t size)
    {
        while (!index.check(static_cast<EntityId>(from)))
        {
            if (from >= size)
            {
                break;
            }

            ++from;
        }

        return from;
    }
}

SparseIndex::Position SparseIndex::bitPos(uint32_t pos)
{
    uint32_t index = pos / k_bitsPerBlock;
    uint32_t bit = pos - (index * k_bitsPerBlock);

    assert(bit < k_bitsPerBlock && "Bit index out of range");

    return { index, static_cast<unsigned>(bit) };
}

void SparseIndex::insert(EntityId id)
{
    auto pos = bitPos(id);

    while (pos.index >= m_bits.size())
    {
        allocateBlock();
    }

    m_bits[pos.index].set(pos.bit);
}

void SparseIndex::erase(EntityId id)
{
    auto pos = bitPos(id);

    if (pos.index >= m_bits.size())
    {
        return;
    }

    m_bits[pos.index].reset(pos.bit);
}

void SparseIndex::clear()
{
    m_bits.clear();
}

bool SparseIndex::check(EntityId id) const
{
    auto pos = bitPos(id);

    if (pos.index >= m_bits.size())
    {
        return false;
    }

    return m_bits[pos.index][pos.bit];
}

size_t SparseIndex::size() const
{
    size_t count = 0;

    for (auto& bitset : m_bits)
    {
        count += bitset.count();
    }

    return count;
}

bool SparseIndex::empty() const
{
    return size() == 0u;
}

SparseIndex& SparseIndex::operator|=(const SparseIndex& other)
{
    size_t otherSize = other.m_bits.size();
    while (otherSize > m_bits.size())
    {
        // Ensure we have enough blocks for ORing against other
        allocateBlock();
    }

    size_t thisSize = m_bits.size();
    size_t maxSize = (std::max)(thisSize, otherSize);

    for (size_t i = 0; i < maxSize; ++i)
    {
        if (i >= otherSize)
        {
            // Other has no more bits to OR
            break;
        }

        m_bits[i] |= other.m_bits[i];
    }

    return *this;
}

SparseIndex& SparseIndex::operator&=(const SparseIndex& other)
{
    size_t thisSize = m_bits.size();
    size_t otherSize = other.m_bits.size();
    size_t maxSize = (std::max)(thisSize, otherSize);

    for (size_t i = 0; i < maxSize; ++i)
    {
        if (i >= thisSize)
        {
            // We have no more bits to AND
            break;
        }

        if (i >= otherSize)
        {
            // Other has no more bits, reset our 
            // remaining bits because we're ANDing
            m_bits[i].reset();
        }
        else
        {
            m_bits[i] &= other.m_bits[i];
        }
    }

    return *this;
}

SparseIndex& SparseIndex::operator^=(const SparseIndex& other)
{
    size_t otherSize = other.m_bits.size();
    while (otherSize > m_bits.size())
    {
        // Ensure we have enough blocks for XORing against other
        allocateBlock();
    }

    size_t thisSize = m_bits.size();
    size_t maxSize = (std::max)(thisSize, otherSize);

    bool otherHasNoMoreBits = false;

    for (size_t i = 0; i < maxSize; ++i)
    {
        if (i >= otherSize)
        {
            otherHasNoMoreBits = true;
        }

        if (otherHasNoMoreBits)
        {
            // Other has no more bits, so XOR our 
            // remaining bits against empty bits
            m_bits[i] ^= DataBlock();
        }
        else
        {
            m_bits[i] ^= other.m_bits[i];
        }
    }

    return *this;
}

void SparseIndex::allocateBlock()
{
    m_bits.emplace_back(DataBlock());
}

SparseIndex::Iterator SparseIndex::begin() const
{
    size_t size = m_bits.size() * k_bitsPerBlock;
    size_t pos = skipEmptyBits(*this, 0u, size);

    return Iterator(*this, size, pos);
}

SparseIndex::Iterator SparseIndex::end() const
{
    size_t size = m_bits.size() * k_bitsPerBlock;

    return Iterator(*this, size, size);
}

SparseIndex::Iterator& SparseIndex::Iterator::operator++()
{
    m_pos = skipEmptyBits(m_container, ++m_pos, m_size);
    return *this;
}

SparseIndex::Iterator SparseIndex::Iterator::operator++(int)
{
    Iterator it = *this;
    ++(*this);
    return it;
}

bool SparseIndex::Iterator::operator==(Iterator other) const
{
    return m_pos == other.m_pos;
}

bool SparseIndex::Iterator::operator!=(Iterator other) const
{
    return !(*this == other);
}

SparseIndex::Iterator::value_type SparseIndex::Iterator::operator*() const
{
    return static_cast<SparseIndex::Iterator::value_type>(m_pos);
}

jaccen::SparseIndex jaccen::operator|(const SparseIndex& lhs, const SparseIndex& rhs)
{
    // TODO: Share code with member operator overload

    SparseIndex out;
    size_t lhsSize = lhs.m_bits.size();
    size_t rhsSize = rhs.m_bits.size();
    size_t maxSize = (std::max)(lhsSize, rhsSize);

    for (size_t i = 0; i < maxSize; ++i)
    {
        out.allocateBlock();

        if (i >= lhsSize)
        {
            out.m_bits[i] = rhs.m_bits[i];
        }
        else if (i >= rhsSize)
        {
            out.m_bits[i] = lhs.m_bits[i];
        }
        else
        {
            out.m_bits[i] = lhs.m_bits[i] | rhs.m_bits[i];
        }
    }

    return out;
}

jaccen::SparseIndex jaccen::operator&(const SparseIndex& lhs, const SparseIndex& rhs)
{
    // TODO: Share code with member operator overload

    SparseIndex out;
    size_t lhsSize = lhs.m_bits.size();
    size_t rhsSize = rhs.m_bits.size();
    size_t maxSize = (std::max)(lhsSize, rhsSize);

    for (size_t i = 0; i < maxSize; ++i)
    {
        out.allocateBlock();

        if (i >= lhsSize)
        {
            break;
        }
        else if (i >= rhsSize)
        {
            break;
        }
        else
        {
            out.m_bits[i] = lhs.m_bits[i] & rhs.m_bits[i];
        }
    }

    return out;
}

jaccen::SparseIndex jaccen::operator^(const SparseIndex& lhs, const SparseIndex& rhs)
{
    // TODO: Share code with member operator overload

    constexpr auto emptyBits = SparseIndex::DataBlock();

    SparseIndex out;
    size_t lhsSize = lhs.m_bits.size();
    size_t rhsSize = rhs.m_bits.size();
    size_t maxSize = (std::max)(lhsSize, rhsSize);

    for (size_t i = 0; i < maxSize; ++i)
    {
        out.allocateBlock();

        if (i >= lhsSize)
        {
            out.m_bits[i] = emptyBits ^ rhs.m_bits[i];
        }
        else if (i >= rhsSize)
        {
            out.m_bits[i] = lhs.m_bits[i] ^ emptyBits;
        }
        else
        {
            out.m_bits[i] = lhs.m_bits[i] ^ rhs.m_bits[i];
        }
    }

    return out;
}