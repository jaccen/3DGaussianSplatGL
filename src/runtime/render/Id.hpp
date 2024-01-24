#pragma once

namespace jaccen
{
    namespace gfx
    {
        // 32-bit unsigned integer type which invalides itself on move.
        // todo: rename to InvalidateOnMoveId?
        // todo: merge with Shader and Texture?
        // todo: make template?
        struct Id
        {
            static constexpr unsigned int Invalid = 0u;

        public:
            Id(unsigned int value) : m_value(value) {}
            ~Id() = default;
            Id(const Id&) = default;
            Id& operator=(const Id&) = default;
            Id(Id&& other) noexcept : m_value(other.m_value)
            {
                other.m_value = Invalid;
            }
            Id& operator=(Id&& other) noexcept
            {
                if (&other != this)
                {
                    m_value = other.m_value;
                    other.m_value = Invalid;
                }
                return *this;
            }

            operator unsigned int() const { return m_value; }
            bool valid() const { return m_value != Invalid; }
            unsigned int& uint() { return m_value; }

        private:
            unsigned int m_value = Invalid;
        };
    }
}
