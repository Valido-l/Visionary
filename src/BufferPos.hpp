#pragma once

#include <limits>
#include <cstdint>

struct BufferPos {
    constexpr BufferPos(size_t row = 0, size_t col = 0) : m_Row(row), m_Col(col) {}

    constexpr BufferPos operator+(const BufferPos& other) const noexcept {
        return { m_Row + other.m_Row, m_Col + other.m_Col };
    }

    constexpr bool operator==(const BufferPos& other) const noexcept {
        return m_Row == other.m_Row && m_Col == other.m_Col;
    }

    constexpr bool operator!=(const BufferPos& other) const noexcept {
        return !(*this == other);
    }

    constexpr bool operator<(const BufferPos& other) const noexcept {
        return (m_Row == other.m_Row && m_Col < other.m_Col) || (m_Row < other.m_Row);
    }

    constexpr bool operator>=(const BufferPos& other) const noexcept {
        return !(*this < other);
    }

    constexpr bool operator>(const BufferPos& other) const noexcept {
        return !(*this < other) && *this != other;
    }

    constexpr bool operator<=(const BufferPos& other) const noexcept {
        return !(*this > other);
    }

    constexpr static BufferPos NPos() noexcept {
        return { invalidIndex, invalidIndex };
    }

    constexpr static size_t invalidIndex = std::numeric_limits<size_t>::max();

    size_t m_Row, m_Col;
};
