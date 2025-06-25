#include "Text.h"

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
    window.draw(m_Text);
}

void Text::Update(double deltaTime) {}

void Text::OnTransformChanged() {
    m_Text.setPosition(m_Position);
}

void Text::SetString(const std::string& str) {
    m_Text.setString(str);
}

sf::Vector2f Text::FindCharacterPos(size_t index) {
    return m_Text.findCharacterPos(index);
}