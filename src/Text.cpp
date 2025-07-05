#include "Text.h"
#include <optional>

constexpr uint64_t TEXT_SIZE = 30;

// Helper. Used to get the default text for drawing. 
static sf::Text& GetTextBase() {
    static bool firstCall = true;
    static sf::Font font;

    if (firstCall && !font.openFromFile("Fonts/anon.ttf"))
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
    for(const auto& [index, highlight] : m_HighlightCache)
        window.draw(highlight);

    for(const auto& text : m_Text)
        window.draw(text);
}

void Text::Update(double deltaTime) {}

void Text::OnTransformChanged() {
    for (size_t i = 0; i < m_Text.size(); i++)
        m_Text[i].setPosition({ m_Position.x, m_Position.y + TEXT_SIZE * i });
}

void Text::SetString(const std::vector<std::string>& strVec) {
    m_Text.clear();

    size_t index = 0;
    for (const auto& str : strVec) {
        if (index++ >= 100)
            break;
        sf::Text text(GetTextBase());
        text.setString(str);
        text.setPosition({ m_Position.x, m_Position.y + TEXT_SIZE * m_Text.size() });
        m_Text.emplace_back(text);
    }

    ClearHighlight();
}

sf::Vector2f Text::FindCharacterPos(size_t row, size_t col) const {
    if (m_Text.empty() || col > m_Text.size())
        return m_Position;

    return m_Text.at(col).findCharacterPos(row);
}

void Text::ClearHighlight() noexcept {
    m_HighlightCache.clear();
}

void Text::Highlight(size_t begin, size_t end) noexcept {
    //if(begin >= end || begin > m_StringSize || end > m_StringSize)
    //    return;
    //// Clear any highlights in the cache that are outside begin-end.
    //for (size_t i = 0; i < begin; i++)
    //    m_HighlightCache.erase(i);

    //for (size_t i = end; i < m_StringSize; i++)
    //    m_HighlightCache.erase(i);
    //
    //for(size_type i = begin; i < end; i++) {
    //    if(m_HighlightCache.find(i) != m_HighlightCache.cend())
    //        continue;

    //    sf::Vector2 startPos = FindCharacterPos(i), endPos = FindCharacterPos(i + 1);

    //    if(startPos.y != endPos.y)
    //        continue;

    //    sf::RectangleShape shape({endPos.x - startPos.x, TEXT_SIZE});
    //    shape.setPosition(startPos);
    //    shape.setFillColor(sf::Color::Blue);

    //    m_HighlightCache[i] = shape;
    //}
}