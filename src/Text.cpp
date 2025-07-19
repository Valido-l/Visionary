#include <optional>

#include "FontManager.hpp"
#include "TextBox.h"
#include "Text.h"

Text::Text(TextBox* owner) : m_Owner(owner), m_Text(), m_Highlights() {
    updateText();
}

void Text::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    for (const auto& highlight : m_Highlights)
        target.draw(highlight, states);

    for (const auto& font : m_Text)
        target.draw(font, states);
}

void Text::update(double deltaTime) {}

void Text::onTransformChanged(sf::Vector2f oldPos, sf::Vector2f oldSize) {
    sf::Vector2f deltaPos = m_Position - oldPos;

    for (size_t i = 0; i < m_Text.size(); i++)
        m_Text[i].move(deltaPos);
}

sf::Text Text::buildText(const std::string& str, uint32_t fontSize, sf::Vector2f pos, const sf::Color& color) const noexcept {
    sf::Text text(FontManager::getFont());
    text.setString(str);
    text.setPosition(pos);
    text.setFillColor(color);
    text.setCharacterSize(fontSize);

    return text;
}

void Text::updateText() {
    if (!m_Owner)
        return;

    const auto& ownerTheme = m_Owner->getTheme();
    float lineMargin = ownerTheme.lineMargin;
    uint32_t fontSize = ownerTheme.fontSize;
    const sf::Color& textColor = ownerTheme.textColor;

    // Get the required variables to determine if the text is in frame. 
    float viewYOffset = m_Owner->getPosition().y + m_Owner->getScroll().y;
    float currentHeight = m_Size.y;
    const auto& buffer = m_Owner->getBuffer();

    // Clearing and rebuilding the text each time updateText is called
    // might seem inefficient, but we're only rebuilding at most a few 
    // dozen or so objects, so it won't be too inefficent. 
    m_Text.clear();

    for (size_t i = 0; i < buffer.size(); i++) {
        sf::Vector2 pos = { m_Position.x, m_Position.y + (lineMargin + fontSize) * i };

        // Do not add any text that is out of frame. 
        if (pos.y < viewYOffset - currentHeight || pos.y > viewYOffset + currentHeight)
            continue;
        
        const auto& line = buffer[i];

        m_Text.emplace_back(buildText(line, fontSize, pos, textColor));
    }
}

sf::Vector2f Text::findCharacterPos(CursorLocation pos) const {
    if (!m_Owner)
        return m_Position;
    
    const auto& ownerTheme = m_Owner->getTheme();
    float lineMargin = ownerTheme.lineMargin;
    uint32_t fontSize = ownerTheme.fontSize;
    const sf::Color& textColor = ownerTheme.textColor;

    auto [row, col] = pos;

    // 'Simulate' the text, because it might not actually exist yet.
    auto line = m_Owner->line(row);
    if (!line.has_value())
        return m_Position;
       
    // Create a new text object and place it where it would be, if it existed. 
    sf::Vector2 strPos = { m_Position.x, m_Position.y + (lineMargin + fontSize) * row };
    return buildText(line.value(), fontSize,strPos, sf::Color::Black).findCharacterPos(col);
}

void Text::clearHighlight() noexcept {
    m_Highlights.clear();
}

void Text::highlight(CursorLocation begin, CursorLocation end) noexcept {
    clearHighlight();

    if (!m_Owner || begin >= end)
        return;

    const auto& ownerTheme = m_Owner->getTheme();
    uint32_t fontSize = ownerTheme.fontSize;
    const auto& highlightColor = ownerTheme.selectedTextColor;

    float yCenter = m_Owner->getPosition().y + m_Owner->getScroll().y;
    float currentHeight = m_Size.y;

    auto [beginRow, beginCol]   = begin;
    auto [endRow, endCol]       = end;

    // Helper to get a highligt for single-line positions. 
    static const auto getHighlight = 
        [fontSize, highlightColor](sf::Vector2f startPos, sf::Vector2f endPos) -> sf::RectangleShape {
        sf::RectangleShape shape({ endPos.x - startPos.x, static_cast<float>(fontSize) });
        shape.setPosition(startPos);
        shape.setFillColor(highlightColor);
        return shape;
    };

    if (beginRow == endRow) {
        // Case 1. Same line.
        // Only highlight the characters in between beginCol and endCol.
        m_Highlights.push_back(getHighlight(findCharacterPos(begin), findCharacterPos(end)));
    }
    else {
        // Case 2. Different lines.
        // 1. Highlight from beginCol to the end of the beginLine.
        // 2. Highlight from the beginning of the endLine to endCol. 
        // 3. Highlight everything inbetween beginRow + 1 and endRow - 1.

        // 1. 
        // We use invalidIndex, as any out-of-bounds index gets the
        // position of the last character in the line. 
        m_Highlights.push_back(getHighlight(findCharacterPos(begin),
                                              findCharacterPos({begin.m_Row, CursorLocation::invalidIndex })));

        // 2.
        m_Highlights.push_back(getHighlight(findCharacterPos({end.m_Row, 0}),
                                              findCharacterPos(end)));

        // 3. 
        for (size_t i = beginRow + 1; i < endRow; i++) {
            // Make sure the highlight is in frame.
            // TODO: What about culling 1. and 2.?
            float highlightY = m_Position.y + fontSize * i;
            if (highlightY < yCenter - currentHeight || highlightY > yCenter + currentHeight)
                continue;

            m_Highlights.push_back(getHighlight(findCharacterPos({i, 0}),
                                                  findCharacterPos({i, CursorLocation::invalidIndex})));
        }
    }
}