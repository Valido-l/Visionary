#include <optional>

#include "Text.h"
#include "TextBox.h"

constexpr uint64_t TEXT_SIZE = 30;
constexpr uint64_t CHUNK_SIZE = 256;

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

Text::Text(TextBox* owner) : m_Owner(owner), m_Text(), m_Highlights() {
    UpdateText();
}

void Text::Draw(sf::RenderWindow& window) const {
    for (const auto& highlight : m_Highlights)
        window.draw(highlight);

    for (const auto& text : m_Text)
        window.draw(text);
}

void Text::Update(double deltaTime) {}

void Text::OnTransformChanged() {
    for (size_t i = 0; i < m_Text.size(); i++)
        m_Text[i].setPosition({ m_Position.x, m_Position.y + TEXT_SIZE * i });
}

void Text::UpdateText() {
    if (!m_Owner)
        return;

    // Get the required variables to determine if the text is in frame. 
    float viewYOffset = m_Owner->GetPosition().y + m_Owner->GetScroll().y;
    float currentHeight = m_Size.y;
    const auto& buffer = m_Owner->GetBuffer();

    // Clearing and rebuilding the text each time UpdateText is called
    // might seem inefficient, but we're only rebuilding at most a few 
    // dozen or so objects, so it won't be too inefficent. 
    m_Text.clear();

    for (size_t i = 0; i < buffer.size(); i++) {
        sf::Vector2 pos = { m_Position.x, m_Position.y + TEXT_SIZE * i };

        // Do not add any text that is out of frame. 
        if (pos.y < viewYOffset - currentHeight || pos.y > viewYOffset + currentHeight)
            continue;
        
        const auto& line = buffer[i];

        sf::Text text(GetTextBase());
        text.setString(line);
        text.setPosition(pos);
        m_Text.emplace_back(text);
    }
}

sf::Vector2f Text::FindCharacterPos(CursorLocation pos) const {
    if (!m_Owner)
        return m_Position;
    
    auto [row, col] = pos;

    // 'Simulate' the text, because it might not actually exist yet.
    auto line = m_Owner->Line(row);
    if (!line.has_value())
        return m_Position;
       
    // Create a new text object and place it where it would be, if it existed. 
    sf::Vector2 strPos = { m_Position.x, m_Position.y + TEXT_SIZE * row };
    sf::Text text(GetTextBase());
    text.setString(line.value());
    text.setPosition(strPos);

    return text.findCharacterPos(col);
}

void Text::ClearHighlight() noexcept {
    m_Highlights.clear();
}

void Text::Highlight(CursorLocation begin, CursorLocation end) noexcept {
    ClearHighlight();

    if (!m_Owner || begin >= end)
        return;

    float yCenter = m_Owner->GetPosition().y + m_Owner->GetScroll().y;
    float currentHeight = m_Size.y;

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
        m_Highlights.push_back(getHighlight(FindCharacterPos(begin), FindCharacterPos(end)));
    }
    else {
        // Case 2. Different lines.
        // 1. Highlight from beginCol to the end of the beginLine.
        // 2. Highlight from the beginning of the endLine to endCol. 
        // 3. Highlight everything inbetween beginRow + 1 and endRow - 1.

        // 1. 
        // We use invalidIndex, as any out-of-bounds index gets the
        // position of the last character in the line. 
        m_Highlights.push_back(getHighlight(FindCharacterPos(begin),
                                              FindCharacterPos({begin.m_Row, CursorLocation::invalidIndex })));

        // 2.
        m_Highlights.push_back(getHighlight(FindCharacterPos({end.m_Row, 0}),
                                              FindCharacterPos(end)));

        // 3. 
        for (size_t i = beginRow + 1; i < endRow; i++) {
            // Make sure the highlight is in frame.
            // TODO: What about culling 1. and 2.?
            float highlightY = m_Position.y + TEXT_SIZE * i;
            if (highlightY < yCenter - currentHeight || highlightY > yCenter + currentHeight)
                continue;

            m_Highlights.push_back(getHighlight(FindCharacterPos({i, 0}),
                                                  FindCharacterPos({i, CursorLocation::invalidIndex})));
        }
    }
}