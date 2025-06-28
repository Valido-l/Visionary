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

Text::Text(const std::string& str, sf::Vector2f pos) : m_Text() {
    SetString(str); SetPosition(pos);
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

void Text::SetString(const std::string& str) {
    m_Text.clear();

    for (size_type i = 0; i < str.size(); i++) {
        size_type j = i;
        for (; j < str.size(); j++)
            if (str[j] == '\n')
                break;

        sf::Text text(GetTextBase());
        text.setString(str.substr(i, j - i + 1)); i = j;
        text.setPosition({ m_Position.x, m_Position.y + TEXT_SIZE * m_Text.size() });
        m_Text.emplace_back(text);
    }

    m_StringSize = str.size();

    ClearHighlight();
}

sf::Vector2f Text::FindCharacterPos(size_type index) const {
    if (m_Text.empty())
        return m_Position;

    size_type total = 0, line = 0;
    for (; line < m_Text.size(); line++) {
        size_type strLength = m_Text.at(line).getString().getSize();

        if (total + strLength>= index) 
            return m_Text[line].findCharacterPos(index - total);

        total += strLength;
    }

    return m_Text.back().findCharacterPos(std::string::npos);
}

void Text::ClearHighlight() noexcept {
    m_HighlightCache.clear();
}

void Text::Highlight(size_type begin, size_type end) noexcept {
    if(begin >= end || begin > m_StringSize || end > m_StringSize)
        return;
    // Clear any highlights in the cache that are outside begin-end.
    for (size_t i = 0; i < begin; i++)
        m_HighlightCache.erase(i);

    for (size_t i = end; i < m_StringSize; i++)
        m_HighlightCache.erase(i);
    
    for(size_type i = begin; i < end; i++) {
        if(m_HighlightCache.find(i) != m_HighlightCache.cend())
            continue;

        sf::Vector2 startPos = FindCharacterPos(i), endPos = FindCharacterPos(i + 1);

        if(startPos.y != endPos.y)
            continue;

        sf::RectangleShape shape({endPos.x - startPos.x, TEXT_SIZE});
        shape.setPosition(startPos);
        shape.setFillColor(sf::Color::Blue);

        m_HighlightCache[i] = shape;
    }
}