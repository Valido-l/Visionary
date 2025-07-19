#include <algorithm>

#include "TextBox.h"

TextBox::TextBox(sf::Vector2f pos, sf::Vector2f size) :
                    m_Buffer({ "" }), m_SelectPos(CursorLocation::npos()),
                    m_Cursor(this), m_Text(this), m_LineIndicator(this),
                    m_Background(size), m_LineHighlight(), m_Scroll(0.f, 0.f), 
                    m_ShouldUpdateView(true), m_ShouldUpdateScroll(true) {

    setPosition(pos); setSize(size);

    m_Background.setFillColor(m_Theme.backgroundColor);
    m_LineHighlight.setFillColor(m_Theme.lineHighlightColor);

    add(Config::Get().defaultText);
}

void TextBox::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    const auto& oldView = target.getView();
    sf::Vector2u windowSize = target.getSize();

    sf::View textBoxView(m_Size / 2.0f, m_Size);
    textBoxView.setViewport({ {m_Position.x / windowSize.x, m_Position.y / windowSize.y}, {m_Size.x / windowSize.x, m_Size.y / windowSize.y} });
    textBoxView.move(m_Position + m_Scroll);

    target.setView(textBoxView);

    target.draw(m_Background, states);
    target.draw(m_Text, states);
    target.draw(m_Cursor, states);
    target.draw(m_LineIndicator, states);
    target.draw(m_LineHighlight, states);

    target.setView(oldView);
}

void TextBox::update(double deltaTime) noexcept {
    m_Cursor.update(deltaTime);
    m_Text.update(deltaTime); 

    updateView();
    updateScroll();
}

void TextBox::updateElements() {
    m_Text.updateText();
    m_LineIndicator.updateLines();

    if (isSelecting())
        m_Text.highlight(std::min(m_SelectPos, getCursorLocation()), std::max(m_SelectPos, getCursorLocation()));
    else
        m_Text.clearHighlight(); // Prevent highlight from drawing after we've stopped selecting. 

    // Set the position of the text. 
    // Add the padding and the LineIndicator's text, so that it isn't covered.
    m_Text.setPosition(m_Position + sf::Vector2f(m_Theme.lineIndicatorPad + m_LineIndicator.getSize().x, 0));
}

void TextBox::onTransformChanged(sf::Vector2f oldPos, sf::Vector2f oldSize) {
    updateElements();

    // LineIndicator carries the position of the TextBox. 
    m_LineIndicator.setPosition(m_Position);

    m_Text.setSize(m_Size);
    m_Background.setSize(m_Size);

    // Make sure LineIndicator and LineHighlight fill the width and height respectively. 
    m_LineIndicator.setSize({ m_LineIndicator.getSize().x, m_Size.y });
    m_LineHighlight.setSize({ m_Size.x, static_cast<float>(m_Theme.fontSize) });

    m_ShouldUpdateView = true; m_ShouldUpdateScroll = true;
}

void TextBox::updateView() {
    if (!m_ShouldUpdateView)
        return;

    sf::Vector2f newCursorPos = m_Text.findCharacterPos(m_Cursor.current());
    m_Cursor.setPosition(newCursorPos);

    // Only ensure the cursor's visibility if a scroll update isn't already queued.
    // This prevents the cursor visibility from overriding the scroll update. 
    if (!m_ShouldUpdateScroll)
        ensureCursorVisibility();

    updateElements();

    // Prevent the background and highlight from going out of frame.  
    m_LineHighlight.setPosition({ m_Position.x + m_Scroll.x, newCursorPos.y });
    m_Background.setPosition(m_Position + m_Scroll);

    m_ShouldUpdateView = false;
}

void TextBox::updateScroll() {
    if (!m_ShouldUpdateScroll)
        return;

    updateElements();

    // We only care about the background when updating.
    // The view itself is moved after it is created in Draw().
    m_Background.setPosition(m_Position + m_Scroll);

    m_ShouldUpdateScroll = false;
}

void TextBox::scrollUp() noexcept {
    uint32_t fontSize = m_Theme.fontSize;
    if (m_Scroll.y > fontSize)
        m_Scroll.y -= fontSize;
    else
        m_Scroll.y = 0;

    m_ShouldUpdateScroll = true;
}

void TextBox::scrollDown() noexcept {
    uint32_t fontSize = m_Theme.fontSize;
    float limit = fontSize * (getLineCount() - 1);
    if (m_Scroll.y < limit)
        m_Scroll.y += fontSize;
    else
        m_Scroll.y = limit;

    m_ShouldUpdateScroll = true;
}

void TextBox::ensureCursorVisibility() noexcept {
    auto [cursorX, cursorY] = m_Cursor.getPosition();
    auto [cursorWidth, cursorHeight] = m_Cursor.getSize();
    auto [textBoxX, textBoxY] = m_Position;
    auto [textBoxWidth, textBoxHeight] = m_Size;
    auto [lineIndicatorWidth, lineIndicatorHeight] = m_LineIndicator.getSize();
    auto lineIndicatorPad = m_Theme.lineIndicatorPad;
    auto& [scrollX, scrollY] = m_Scroll;

    if (cursorY + cursorHeight - textBoxHeight > scrollY) {
        scrollY = cursorY - textBoxHeight + cursorHeight;
    }
    if (cursorY - textBoxY < scrollY) {
        scrollY = cursorY - textBoxY;
    }

    if (cursorX + cursorWidth - textBoxWidth > scrollX) {
        scrollX = cursorX + cursorWidth - textBoxWidth;
    }
    if (cursorX - textBoxX - lineIndicatorWidth - lineIndicatorPad < scrollX) {
        scrollX = cursorX - textBoxX - lineIndicatorWidth - lineIndicatorPad;
    }
}

const std::vector<std::string>& TextBox::getBuffer() const noexcept {
    return m_Buffer;
}

sf::Vector2f TextBox::getScroll() const noexcept {
    return m_Scroll;
}

std::optional<std::string> TextBox::line(size_t row) const noexcept {
    if (row >= m_Buffer.size())
        return std::nullopt;

    return m_Buffer.at(row);
}

CursorLocation TextBox::getCursorLocation() const noexcept {
    return m_Cursor.current();
}

size_t TextBox::getLineCount() const noexcept {
    return m_Buffer.size();
}

std::optional<char> TextBox::getCharAt(const CursorLocation& pos) const noexcept {
    auto [row, col] = pos;

    // Make sure the position is within bounds.
    // Check if the row isn't bigger than lineCount
    // and the column isn't bigger than the line's size.
    if (row >= getLineCount() || col >= m_Buffer.at(row).size())
        return std::nullopt;

    return m_Buffer.at(row).at(col);
}

std::optional<char> TextBox::getRightChar() const noexcept {
    return getCharAt(getCursorLocation());
}

std::optional<char> TextBox::getLeftChar() const noexcept {
    return getCharAt(m_Cursor.prev());
}

void TextBox::add(char c) noexcept {
    clearSelection();

    auto [row, col] = getCursorLocation();
    auto& line = m_Buffer[row];

    // Insert an implicit newline.
    if (c == '\n') {
        // 1. Split the line at the cursor's position. 
        // 3. Insert a new line and append the tail.
        // 4. Move to the start of this new line. 
        std::string tail = std::string(line.begin() + col, line.end());
        line.erase(line.begin() + col, line.end()); // Erase the tail, so it isn't duplicated.

        m_Buffer.insert(m_Buffer.begin() + row + 1, {tail});

        moveTo({ row + 1, 0 });

        return;
    }

    // Insert a character normally, make sure it is valid.
    if (!std::isprint(c))
        return;

    line.insert(line.begin() + col, c);
    moveRight();
}

void TextBox::add(const std::string& str) noexcept {
    for(const char c : str)
        add(c);
}

void TextBox::addTab() noexcept {
    for(size_t i = 0; i < Config::Get().tabWidth; i++)
        add(' ');
}

bool TextBox::remove() noexcept {
    if(clearSelection()) {
        return true;
    }

    // Do nothing if the cursor is on the first character.
    if (m_Cursor.onFirstPos())
        return false;

    auto [row, col] = getCursorLocation();

    // We're on the start of the line, delete the implicit new line. 
    if (m_Cursor.onStartLine())
        return removeRange({ row - 1, m_Buffer[row - 1].size() }, { row, col });

    // Delete a character normally.
    // -1 because we're deleting the character left of the cursor. 
    return removeRange({ row, col - 1 }, { row, col });
}

bool TextBox::skipRemove() noexcept {
    if(clearSelection()) {
        return true;
    }

    // Save the current cursor position, skip to the left,
    // and delete all characters in between.
    CursorLocation initial = getCursorLocation();
    return skipLeft() && removeRange(getCursorLocation(), initial);
}

bool TextBox::removeRange(CursorLocation begin, CursorLocation end) noexcept {
    // Ensure the range is actually valid. 
    if (begin > m_Cursor.maxPos() ||
        end > m_Cursor.maxPos() ||
        begin > end)
        return false;

    auto [beginRow, beginCol] = begin;
    auto [endRow, endCol] = end;

    // First Case: On the same line.
    if (beginRow == endRow) {
        auto& line = m_Buffer[beginRow];
        line.erase(line.begin() + beginCol, line.begin() + endCol);
    }
    // Second Case: Different lines.
    else {
        // 1. Split the endLine at endCol.
        // 2. Delete everything between (beginRow, beginCol) and (endRow, endCol).
        // 3. Append the tail to beginLine.

        auto& beginLine = m_Buffer[beginRow];
        auto& endLine = m_Buffer[endRow];

        std::string tail = std::string(endLine.begin() + endCol, endLine.end());

        endLine.erase(endLine.begin() + endCol, endLine.end());
        beginLine.erase(beginLine.begin() + beginCol, beginLine.end());

        // Everything in between.
        m_Buffer.erase(m_Buffer.begin() + beginRow + 1, m_Buffer.begin() + endRow + 1);
 
        beginLine.append(tail);
    }

    return moveTo(begin);
}

bool TextBox::removeTab() noexcept {
    for (size_t i = 0; i <= Config::Get().tabWidth; i++) {
        if (getLeftChar() == ' ')
            remove();
        else
            return i != 0;
    }

    return true;
}

bool TextBox::isSelecting() const noexcept {
    return m_SelectPos != CursorLocation::npos();
}

void TextBox::startSelecting() noexcept {
    m_SelectPos = getCursorLocation();
    m_ShouldUpdateView = true;
}

void TextBox::stopSelecting() noexcept {
    m_SelectPos = CursorLocation::npos();
    m_ShouldUpdateView = true;
}

bool TextBox::clearSelection() noexcept {
    if(!isSelecting())
        return false;

    // TODO: I use this in 3 places, maybe make it a function?
    removeRange(std::min(m_SelectPos, getCursorLocation()), std::max(m_SelectPos, getCursorLocation()));
    stopSelecting();
    return true;
}

std::optional<std::string> TextBox::getSelection() const noexcept {
    if(!isSelecting())
        return std::nullopt;

    auto [beginRow, beginCol] = std::min(m_SelectPos, getCursorLocation());
    auto [endRow, endCol] = std::max(m_SelectPos, getCursorLocation());

    // TODO: I have this similar same line / different line pattern in 4 different places.
    // Maybe think of a way to reduce code repetition. 
    
    // First Case: On the same line.
    if (beginRow == endRow) {
        auto& line = m_Buffer[beginRow];
        return std::string(line.begin() + beginCol, line.begin() + endCol);
    }
    // Second Case: Different lines.
    else {
        // 1. Split the beginLine at beginCol and append it to ret.
        // 2. Append everything between (beginRow, beginCol) and (endRow, endCol).
        // 3. Split the endLine at endCol and append it to ret.

        // Build the string.
        std::string ret = "";
        auto& beginLine = m_Buffer[beginRow];
        auto& endLine = m_Buffer[endRow];

        ret += std::string(beginLine.begin() + beginCol, beginLine.end()) + '\n';

        for (size_t i = beginRow + 1; i < endRow; i++) {
            ret += m_Buffer.at(i) + '\n';
        }

        ret += std::string(endLine.begin(), endLine.begin() + endCol);
        return ret;
    }
}

void TextBox::selectAll() noexcept {
    stopSelecting();
    moveTop();
    startSelecting();
    moveBottom();
    m_ShouldUpdateView = true;
}

bool TextBox::moveTo(CursorLocation pos) noexcept {
    m_ShouldUpdateView = true;

    // Cursor is already at the provided pos. 
    if (pos == getCursorLocation())
        return false;

    auto [row, col] = pos;

    // Clamp in case of invalid pos.
    if (row >= m_Buffer.size()) {
        row = m_Buffer.back().size(); col = m_Buffer.size() - 1;
    }
    if (col > m_Buffer[row].size()) {
        col = m_Buffer[row].size();
    }

    return m_Cursor.moveTo({ row, col });
}

bool TextBox::moveUp() noexcept {
   return moveTo(m_Cursor.above());
}

bool TextBox::moveDown() noexcept {
    return moveTo(m_Cursor.below());
}

bool TextBox::moveLeft() noexcept {
    return moveTo(m_Cursor.prev());
}

bool TextBox::moveRight() noexcept {
    return moveTo(m_Cursor.next());
}

void TextBox::moveTop() noexcept {
    moveTo(m_Cursor.minPos());
}

void TextBox::moveBottom() noexcept {
    moveTo(m_Cursor.maxPos());
}

void TextBox::moveStart() noexcept {
    moveTo(m_Cursor.startLinePos());
}

void TextBox::moveEnd() noexcept {
    moveTo(m_Cursor.endLinePos());
}

bool TextBox::skipLeft() noexcept {
    if (m_Cursor.onFirstPos())
        return false;

    // If we're at the start of the line, just move to the left. 
    if (m_Cursor.onStartLine()) {
        moveLeft(); return true;
    }

    char leftChar = getLeftChar().value(); // Must have a value, as we're not on firstPos.

    // Get the closest chars of each class. 
    auto nextLeftSpace = findFirstLeft(isspace);
    auto nextLeftAlnum = findFirstLeft(isalnum);
    auto nextLeftPunct = findFirstLeft(ispunct);

    // A char can be one of three classes.
    // 1. Whitespace: '\n', '\t', ' ', etc.
    // 2. Alphanumeric: 'A', '3', 'h', etc.
    // 3. Punctuation: '*', '.', '+', etc.
    // In any case, just skip to the closest character that isn't of the leftChar's class. 
    // If one was found beyond the confines of the line, skip to the startPos. 
    auto found =    (isspace(leftChar)) ? std::max({ nextLeftAlnum, nextLeftPunct, m_Cursor.startLinePos() }) :
                    (isalnum(leftChar)) ? std::max({ nextLeftSpace, nextLeftPunct, m_Cursor.startLinePos() }) :
                    (ispunct(leftChar)) ? std::max({ nextLeftSpace, nextLeftAlnum, m_Cursor.startLinePos() }) :
                    m_Cursor.minPos();

    return moveTo(found);
}

bool TextBox::skipRight() noexcept {
    // We cannot skip if we're at the end.
    if (m_Cursor.onLastPos())
        return false;

    // If we're at the end of the line, just move to the right. 
    if (m_Cursor.onEndLine()) {
        moveRight(); return true;
    }

    char rightChar = getRightChar().value(); // Must have a value, as we're not on lastPos. 

    // Get the closest chars of each class. 
    auto nextRightSpace = findFirstRight(isspace);
    auto nextRightAlnum = findFirstRight(isalnum);
    auto nextRightPunct = findFirstRight(ispunct);

    // A char can be one of three classes.
    // 1. Whitespace: '\n', '\t', ' ', etc.
    // 2. Alphanumeric: 'A', '3', 'h', etc.
    // 3. Punctuation: '*', '.', '+', etc.
    // In any case, just skip to the closest character that isn't of the rightChar's class. 
    // If one was found beyond the confines of the line, skip to the endPos. 
    auto found = (isspace(rightChar)) ? std::min({ nextRightAlnum, nextRightPunct, m_Cursor.endLinePos() }) :
                 (isalnum(rightChar)) ? std::min({ nextRightSpace, nextRightPunct, m_Cursor.endLinePos() }) :
                 (ispunct(rightChar)) ? std::min({ nextRightSpace, nextRightAlnum, m_Cursor.endLinePos() }) :
                 m_Cursor.maxPos();

    return moveTo(found);
}

CursorLocation TextBox::findFirstLeft(const std::function<bool(char)>& pred) const {
    // Move one position back each iteration and check
    // if the character at that position matches the predicate. 
    for (CursorLocation current = m_Cursor.prev();; current = m_Cursor.prev(current)) {
        auto c = getCharAt(current);

        if (!c.has_value()) // Sanity check. Make sure it has a value. 
            continue;

        if (pred(c.value())) // +1, because we want to move to the *right* of the found character. 
            return current + CursorLocation(0, 1);

        if (current == m_Cursor.minPos())
            break;
    }

    return m_Cursor.minPos();
}

CursorLocation TextBox::findFirstRight(const std::function<bool(char)>& pred) const {
    // Move one position forward each iteration and check
    // if the character at that position matches the predicate. 
    for (CursorLocation current = m_Cursor.next(); current < m_Cursor.maxPos(); current = m_Cursor.next(current)) {
        auto c = getCharAt(current);

        if (!c.has_value()) // Sanity check. Make sure it has a value. 
            continue;

        if (pred(c.value()))
            return current;
    }

    return m_Cursor.maxPos();
}

void TextBox::paste() noexcept {
    add(sf::Clipboard::getString());
}

void TextBox::copy() const noexcept {
    auto selection = getSelection();

    if(selection.has_value())
        sf::Clipboard::setString(static_cast<std::string>(selection.value()));
}