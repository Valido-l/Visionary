#include <string>

#include "Cursor.h"
#include "TextBox.h"

Cursor::Cursor(TextBox* owner) noexcept : 
    m_Owner(owner), m_CursorLocation({0, 0}), m_Shape() {
    m_Shape.setFillColor(m_Theme.cursorColor);
    m_Shape.setOutlineThickness(m_Theme.outlineThickness);
    m_Shape.setOutlineColor(m_Theme.outlineColor);

    if (!m_Owner)
        return;

    setSize({ m_Theme.cursorWidth, static_cast<float>(m_Owner->getTheme().fontSize) });
}

void Cursor::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    target.draw(m_Shape, states);
}

void Cursor::update(double deltaTime) {}

CursorLocation Cursor::current() const noexcept {
    return m_CursorLocation;
}

bool Cursor::moveTo(CursorLocation to) {
    if (!isValidPos(to))
        return false;

    if (m_CursorLocation == to)
        return false;

    m_CursorLocation.m_Row = to.m_Row; m_CursorLocation.m_Col = to.m_Col;
    return true;
}

bool Cursor::isValidPos(CursorLocation pos) const noexcept {
    if (!m_Owner)
        return false;

    return pos <= maxPos();
}

CursorLocation Cursor::above() const noexcept {
    // Make sure the owner exists.
    if (!m_Owner)
        return minPos();

    if (onFirstLine())
        return m_CursorLocation;

    auto [row, col] = m_CursorLocation;

    auto line = m_Owner->line(row - 1);

    if (!line.has_value())
        return m_CursorLocation;

    return { row - 1, std::min(col, line.value().size()) };
}

CursorLocation Cursor::below() const noexcept {
    // Make sure the owner exists.
    if (!m_Owner)
        return minPos();

    if (onLastLine())
        return m_CursorLocation;

    auto [row, col] = m_CursorLocation;

    auto line = m_Owner->line(row + 1);

    if (!line.has_value())
        return m_CursorLocation;

    return { row + 1, std::min(col, line.value().size()) };
}

CursorLocation Cursor::prev(CursorLocation pos) const noexcept {
    // Make sure the owner exists.
    if (!m_Owner)
        return minPos();

    // Make sure the pos is valid.
    if (!isValidPos(pos))
        return minPos();

    // We're on the first possible location.
    if (onFirstPos())
        return minPos();

    auto [row, col] = pos;

    // We're at the first char of the line.
    // Return the location at the end of the previous line.
    if (col == 0) {
        const auto prevLine = m_Owner->line(row - 1);

        if (!prevLine.has_value()) // Sanity check. Make sure the line exists.
            return minPos();

        return { row - 1, prevLine.value().size() };
    }

    // Just return the location one char to the left.
    return { row, col - 1};
}

CursorLocation Cursor::prev() const noexcept {
    return prev(m_CursorLocation);
}

CursorLocation Cursor::next(CursorLocation pos) const noexcept {
    // Make sure the owner exists.
    if (!m_Owner)
        return maxPos();

    // Make sure the pos is valid.
    if (!isValidPos(pos))
        return maxPos();

    // We're on the last possible location.
    if (onLastPos())
        return maxPos();

    auto [row, col] = pos;

    // Check if we're on the last char of the line.
    auto currentLine = m_Owner->line(row);
    if (!currentLine.has_value())
        return maxPos();

    // We're at the first char of the line.
    // Return the location at the start of the next line.
    if (col == currentLine.value().size()) {
        const auto nextLine = m_Owner->line(row + 1);

        if (!nextLine.has_value()) // Sanity check. Make sure the line exists.
            return maxPos();

        return { row + 1, 0 };
    }

    // Just return the location one char to the right.
    return { row, col + 1 };
}

CursorLocation Cursor::next() const noexcept {
    return next(m_CursorLocation);
}

void Cursor::onTransformChanged(sf::Vector2f oldPos, sf::Vector2f oldSize) {
    m_Shape.setPosition(m_Position);
    m_Shape.setSize(m_Size);
}

CursorLocation Cursor::minPos() const noexcept {
    return { 0, 0 };
}

CursorLocation Cursor::maxPos() const noexcept {
    if (!m_Owner)
        return minPos();

    size_t lineCount = m_Owner->getLineCount() - 1;
    auto lastLine = m_Owner->line(lineCount);

    if (!lastLine.has_value())
        return minPos();

    return { lineCount, lastLine.value().size() };
}

CursorLocation Cursor::startLinePos() const noexcept {
    if (!m_Owner)
        return minPos();

    size_t row = m_CursorLocation.m_Row;
    return { row, 0 };
}

CursorLocation Cursor::endLinePos() const noexcept {
    if (!m_Owner)
        return minPos();

    size_t row = m_CursorLocation.m_Row;
    auto lastLine = m_Owner->line(row);

    if (!lastLine.has_value())
        return minPos();

    return { row, lastLine.value().size() };
}

bool Cursor::onFirstLine() const noexcept {
    return m_CursorLocation.m_Row == 0;
}

bool Cursor::onLastLine() const noexcept {
    return m_CursorLocation.m_Row == maxPos().m_Row;
}

bool Cursor::onStartLine() const noexcept {
    return m_CursorLocation.m_Col == 0;
}

bool Cursor::onEndLine() const noexcept {
    return m_CursorLocation.m_Col == endLinePos().m_Col;
}

bool Cursor::onFirstPos() const noexcept {
    return onFirstLine() && onStartLine();
}

bool Cursor::onLastPos() const noexcept {
    return onLastLine() && onEndLine();
}