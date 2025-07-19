#include "FontManager.hpp"
#include "LineIndicator.h"
#include "TextBox.h"

LineIndicator::LineIndicator(TextBox* owner, sf::Vector2f pos, sf::Vector2f size) noexcept :
                                m_Owner(owner), m_Background(), m_LineNumbers() {
    setPosition(pos); setSize(size);

    m_Background.setFillColor(m_Theme.backgroundColor);
    m_Background.setOutlineColor(m_Theme.outlineColor);
    m_Background.setOutlineThickness(m_Theme.outlineThickness);
}

void LineIndicator::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    target.draw(m_Background, states);

    for(const auto& lineNumber : m_LineNumbers)
        target.draw(lineNumber, states);
}

void LineIndicator::update(double deltaTime) {}

void LineIndicator::onTransformChanged(sf::Vector2f oldPos, sf::Vector2f oldSize) {
    m_Background.setPosition(m_Position);
    m_Background.setSize(m_Size);

    for (auto& lineNumber : m_LineNumbers)
        lineNumber.setPosition(m_Position + sf::Vector2f(m_Theme.padLeft, 0));
}

void LineIndicator::updateLines() noexcept {
    if (!m_Owner)
        return;

    // Get the properties for the text. 
    const auto& ownerTheme = m_Owner->getTheme();
    float lineMargin = ownerTheme.lineMargin;
    uint32_t fontSize = ownerTheme.fontSize;

    m_LineNumbers.clear();

    // We might be scrolled down, so update the background's pos.
    m_Background.move({ 0, m_Owner->getScroll().y });

    // Get the required variables to determine if the text is in frame. 
    float viewYOffset = m_Owner->getPosition().y + m_Owner->getScroll().y;
    float currentHeight = m_Size.y;

    size_t lineCount = m_Owner->getLineCount();
    size_t maxDigits = std::to_string(lineCount).size();

    // Make sure the container is big to fit the line number with the most digits. 
    setSize({ m_Theme.padLeft + maxDigits * fontSize + m_Theme.padRight, m_Size.y });
    // We might be scrolled down, so update the background's pos.
    m_Background.setPosition({ m_Position.x, m_Position.y + m_Owner->getScroll().y });

    // Add all formatted lines. 
    for (size_t line = 1; line <= lineCount; line++) {
        sf::Vector2 pos = { m_Position.x + m_Theme.padLeft, m_Position.y + (fontSize + lineMargin) * (line - 1) };

        // Do not add any line numbers that are out of frame. 
        if (pos.y < viewYOffset - currentHeight || pos.y > viewYOffset + currentHeight)
            continue;

        // Build the text and insert it in m_LineNumbers. 
        sf::Text lineText(FontManager::getFont());
        lineText.setString(std::to_string(line));
        lineText.setPosition(pos);
        lineText.setFillColor(m_Theme.textColor);
        lineText.setCharacterSize(fontSize);
        m_LineNumbers.push_back(lineText);
    }
}
