#include "Text.h"
#include <optional>

constexpr uint64_t TEXT_SIZE = 30;

// Helper. Used to get the default text for drawing. 
static sf::Text& GetTextBase() {
    static bool firstCall = true;
    static sf::Font font;

    if (firstCall && !font.openFromFile("Fonts/CascadiaCode.ttf"))
        throw std::runtime_error("Cannot load the font.");

    static sf::Text text(font, "", TEXT_SIZE);

    if (firstCall) {
        text.setFillColor(sf::Color::White);
        firstCall = false;
    }

    return text;
}

Text::Text(const std::vector<std::string>& strVec, sf::Vector2f pos) : m_Text() {
    SetString(strVec); SetPosition(pos);
}

void Text::Draw(sf::RenderWindow& window) const {
    for (const auto& highlight : m_HighlightVec)
        window.draw(highlight);

    for (const auto& text : m_Text)
        window.draw(text);
}

void Text::Update(double deltaTime) {}

void Text::OnTransformChanged() {
    for (size_t i = 0; i < m_Text.size(); i++)
        m_Text[i].setPosition({ m_Position.x, m_Position.y + TEXT_SIZE * i });
}

void Text::SetString(const std::vector<std::string>& strVec) {
    m_Text.clear();

    for (const auto& str : strVec) {
        sf::Text text(GetTextBase());
        text.setString(str);
        text.setPosition({ m_Position.x, m_Position.y + TEXT_SIZE * m_Text.size() });
        m_Text.emplace_back(text);
    }

    ClearHighlight();
}

sf::Vector2f Text::FindCharacterPos(BufferPos pos) const {
    auto [row, col] = pos;

    if (m_Text.empty() || row > m_Text.size())
        return m_Position;

    return m_Text.at(row).findCharacterPos(col);
}

void Text::ClearHighlight() noexcept {
    m_HighlightVec.clear();
}

void Text::Highlight(BufferPos begin, BufferPos end) noexcept {
    if (begin >= end)
        return;

    ClearHighlight();

    auto [beginRow, beginCol]   = begin;
    auto [endRow, endCol]       = end;

    // Helper to get a highligt for single-line positions. 
    static const auto getHighlight = [](sf::Vector2f startPos, sf::Vector2f endPos) -> sf::RectangleShape {
        sf::RectangleShape shape({ endPos.x - startPos.x, TEXT_SIZE });
        shape.setPosition(startPos);
        shape.setFillColor(sf::Color::Blue);
        return shape;
    };

    if (beginRow == endRow) {
        // Case 1. Same line.
        // Only highlight the characters in between beginCol and endCol.
        m_HighlightVec.push_back(getHighlight(FindCharacterPos(begin), FindCharacterPos(end)));
    }
    else {
        // Case 2. Different lines.
        // 1. Highlight from beginCol to the end of the beginLine.
        // 2. Highlight from the beginning of the endLine to endCol. 
        // 3. Highlight everything inbetween beginRow + 1 and endRow - 1.

        // 1. 
        // We use invalidIndex, as any out-of-bounds index gets the
        // position of the last character in the line. 
        m_HighlightVec.push_back(getHighlight(FindCharacterPos(begin),
                                              FindCharacterPos({begin.m_Row, BufferPos::invalidIndex })));

        // 2.
        m_HighlightVec.push_back(getHighlight(FindCharacterPos({end.m_Row, 0}),
                                              FindCharacterPos(end)));

        // 3. 
        for (size_t i = beginRow + 1; i < endRow; i++) {
            m_HighlightVec.push_back(getHighlight(FindCharacterPos({i, 0}),
                                                  FindCharacterPos({i, BufferPos::invalidIndex})));
        }

    }
}