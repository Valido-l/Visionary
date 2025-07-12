#include <string>

#include "Cursor.h"
#include "TextBox.h"

Cursor::Cursor(TextBox* owner, sf::Vector2f size, BufferPos pos) noexcept : m_Owner(owner), m_BufferPos(pos), m_Shape() {
    SetPosition({ 0, 0 }); SetSize(size);
    m_Shape.setFillColor(sf::Color::White);
}

void Cursor::Draw(sf::RenderWindow& window) const {
    window.draw(m_Shape);
}

void Cursor::Update(double deltaTime) {}

BufferPos Cursor::Current() const noexcept {
    return m_BufferPos;
}

bool Cursor::MoveTo(BufferPos to) {
    if (!IsValidPos(to))
        return false;

    if (m_BufferPos == to)
        return false;

    m_BufferPos.m_Row = to.m_Row; m_BufferPos.m_Col = to.m_Col;
    return true;
}

bool Cursor::IsValidPos(BufferPos pos) const noexcept {
    if (!m_Owner)
        return false;

    return pos <= MaxPos();
}

BufferPos Cursor::Above() const noexcept {
    // Make sure the owner exists.
    if (!m_Owner)
        return MinPos();

    if (OnFirstLine())
        return m_BufferPos;

    auto [row, col] = m_BufferPos;

    auto line = m_Owner->Line(row - 1);

    if (!line.has_value())
        return m_BufferPos;

    return { row - 1, std::min(col, line.value().size()) };
}

BufferPos Cursor::Below() const noexcept {
    // Make sure the owner exists.
    if (!m_Owner)
        return MinPos();

    if (OnLastLine())
        return m_BufferPos;

    auto [row, col] = m_BufferPos;

    auto line = m_Owner->Line(row + 1);

    if (!line.has_value())
        return m_BufferPos;

    return { row + 1, std::min(col, line.value().size()) };
}

BufferPos Cursor::Prev(BufferPos pos) const noexcept {
    // Make sure the owner exists.
    if (!m_Owner)
        return MinPos();

    // Make sure the pos is valid.
    if (!IsValidPos(pos))
        return MinPos();

    // We're on the first possible position.
    if (OnFirstPos())
        return MinPos();

    auto [row, col] = pos;

    // We're at the first char of the line.
    // Return the position at the end of the previous line.
    if (col == 0) {
        const auto prevLine = m_Owner->Line(row - 1);

        if (!prevLine.has_value()) // Sanity check. Make sure the line exists.
            return MinPos();

        return { row - 1, prevLine.value().size() };
    }

    // Just return the position one char to the left.
    return { row, col - 1};
}

BufferPos Cursor::Prev() const noexcept {
    return Prev(m_BufferPos);
}

BufferPos Cursor::Next(BufferPos pos) const noexcept {
    // Make sure the owner exists.
    if (!m_Owner)
        return MaxPos();

    // Make sure the pos is valid.
    if (!IsValidPos(pos))
        return MaxPos();

    // We're on the last possible position.
    if (OnLastPos())
        return MaxPos();

    auto [row, col] = pos;

    // Check if we're on the last char of the line.
    auto currentLine = m_Owner->Line(row);
    if (!currentLine.has_value())
        return MaxPos();

    // We're at the first char of the line.
    // Return the position at the start of the next line.
    if (col == currentLine.value().size()) {
        const auto nextLine = m_Owner->Line(row + 1);

        if (!nextLine.has_value()) // Sanity check. Make sure the line exists.
            return MaxPos();

        return { row + 1, 0 };
    }

    // Just return the position one char to the right.
    return { row, col + 1 };
}

BufferPos Cursor::Next() const noexcept {
    return Next(m_BufferPos);
}

void Cursor::OnTransformChanged() {
    m_Shape.setPosition(m_Position);
    m_Shape.setSize(m_Size);
}

BufferPos Cursor::MinPos() const noexcept {
    return { 0, 0 };
}

BufferPos Cursor::MaxPos() const noexcept {
    if (!m_Owner)
        return MinPos();

    size_t lineCount = m_Owner->GetLineCount() - 1;
    auto lastLine = m_Owner->Line(lineCount);

    if (!lastLine.has_value())
        return MinPos();

    return { lineCount, lastLine.value().size() };
}

BufferPos Cursor::StartLinePos() const noexcept {
    if (!m_Owner)
        return MinPos();

    size_t row = m_BufferPos.m_Row;
    return { row, 0 };
}

BufferPos Cursor::EndLinePos() const noexcept {
    if (!m_Owner)
        return MinPos();

    size_t row = m_BufferPos.m_Row;
    auto lastLine = m_Owner->Line(row);

    if (!lastLine.has_value())
        return MinPos();

    return { row, lastLine.value().size() };
}

bool Cursor::OnFirstLine() const noexcept {
    return m_BufferPos.m_Row == 0;
}

bool Cursor::OnLastLine() const noexcept {
    return m_BufferPos.m_Row == MaxPos().m_Row;
}

bool Cursor::OnStartLine() const noexcept {
    return m_BufferPos.m_Col == 0;
}

bool Cursor::OnEndLine() const noexcept {
    return m_BufferPos.m_Col == EndLinePos().m_Col;
}

bool Cursor::OnFirstPos() const noexcept {
    return OnFirstLine() && OnStartLine();
}

bool Cursor::OnLastPos() const noexcept {
    return OnLastLine() && OnEndLine();
}