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

Text::Text(const std::string& str, sf::Vector2f pos) : m_Text(GetTextBase()) {
    m_Text.setString(str); SetPosition(pos);
}

void Text::Draw(sf::RenderWindow& window) const {
    for(const auto& [index, highlight] : m_HighlightCache)
        window.draw(highlight);

    window.draw(m_Text);
}

void Text::Update(double deltaTime) {}

void Text::OnTransformChanged() {
    m_Text.setPosition(m_Position);
}

void Text::SetString(const std::string& str) {
    m_Text.setString(str);
    m_StringSize = str.size();

    m_CharacterPosCache.clear();
    for(size_t i = 0; i < m_StringSize; i++)
        m_CharacterPosCache[i] = m_Text.findCharacterPos(i);

    ClearHighlight();
}

sf::Vector2f Text::FindCharacterPos(size_t index) {
    return m_Text.findCharacterPos(index);
}

void Text::ClearHighlight() noexcept {
    m_HighlightCache.clear();
}

void Text::Highlight(size_type begin, size_type end) noexcept {
    FindCharacterPos(m_StringSize - 1);
    if(begin >= end || begin > m_StringSize || end > m_StringSize)
        return;
    
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