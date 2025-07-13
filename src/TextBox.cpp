#include <algorithm>

#include "TextBox.h"

constexpr uint64_t TEXT_SIZE = 30;
constexpr uint64_t TAB_WIDTH = 4;

TextBox::TextBox(sf::Vector2f pos, sf::Vector2f size) : 
                    m_Buffer({"Hello, World!"}), m_SelectPos(CursorLocation::NPos()),
                    m_Cursor(this, { 2.0f, TEXT_SIZE }), m_Text(this), 
                    m_Background(size), m_LineHighlight({size.x, TEXT_SIZE}), m_Scroll(0.f, 0.f), 
                    m_ShouldUpdateString(true), m_ShouldUpdateView(true), m_ShouldUpdateScroll(true) {

    SetPosition(pos); SetSize(size);

    m_Background.setOutlineColor(sf::Color(70, 70, 70, 255));
    m_Background.setFillColor(sf::Color(90, 90, 90, 255));
    m_Background.setOutlineThickness(1.0f);

    m_LineHighlight.setOutlineColor(sf::Color(90, 90, 90, 100));
    m_LineHighlight.setFillColor(sf::Color(50, 50, 50, 100));
    m_LineHighlight.setOutlineThickness(2.0f);
}

void TextBox::Draw(sf::RenderWindow& window) const {    
    const auto oldView = window.getView();
    sf::Vector2u windowSize = window.getSize();                

    sf::View textBoxView(m_Size / 2.0f, m_Size);
    textBoxView.setViewport({{m_Position.x / windowSize.x, m_Position.y / windowSize.y}, {m_Size.x / windowSize.x, m_Size.y / windowSize.y}});
    textBoxView.move(m_Position + m_Scroll);

    window.setView(textBoxView);

    window.draw(m_Background);
    m_Text.Draw(window); 
    m_Cursor.Draw(window);
    window.draw(m_LineHighlight);

    window.setView(oldView); 
}

void TextBox::Update(double deltaTime) noexcept {
    m_Cursor.Update(deltaTime);
    m_Text.Update(deltaTime); 
    
    UpdateString();
    UpdateView();
    UpdateScroll();
}

void TextBox::OnTransformChanged() {
    m_Text.SetPosition(m_Position);
    m_Text.SetSize(m_Size);

    m_Background.setSize(m_Size);
    m_LineHighlight.setSize({m_Size.x, m_LineHighlight.getSize().y});

    m_ShouldUpdateView = true; m_ShouldUpdateScroll = true;
}

void TextBox::UpdateString() {
    if(!m_ShouldUpdateString)
        return;

    m_ShouldUpdateString = false;
}

void TextBox::UpdateView() {
    if(!m_ShouldUpdateView)
        return;
   
    sf::Vector2f newCursorPos = m_Text.FindCharacterPos(m_Cursor.Current());
    m_Cursor.SetPosition(newCursorPos);

    // Only ensure the cursor's visibility if a scroll update isn't already queued.
    // This prevents the cursor visibility from overriding the scroll update. 
    if(!m_ShouldUpdateScroll)
        EnsureCursorVisibility();

    m_Text.UpdateText();
    // Prevent the background and highlight from going out of frame.  
    m_LineHighlight.setPosition({m_Position.x + m_Scroll.x, newCursorPos.y});
    m_Background.setPosition(m_Position + m_Scroll); 

    if(IsSelecting())
        m_Text.Highlight(std::min(m_SelectPos, GetCursorLocation()), std::max(m_SelectPos, GetCursorLocation()));
    else
        m_Text.ClearHighlight(); // Prevent highlight from drawing after we've stopped selecting. 
    
    m_ShouldUpdateView = false;
}

void TextBox::UpdateScroll() {
    if (!m_ShouldUpdateScroll)
        return;

    m_Text.UpdateText();
    // We only care about the background when updating.
    // The view itself is moved after it is created in Draw().
    m_Background.setPosition(m_Position + m_Scroll);

    if (IsSelecting())
        m_Text.Highlight(std::min(m_SelectPos, GetCursorLocation()), std::max(m_SelectPos, GetCursorLocation()));
    else
        m_Text.ClearHighlight(); // Prevent highlight from drawing after we've stopped selecting. 

    m_ShouldUpdateScroll = false;
}

void TextBox::ScrollUp() noexcept {
    if (m_Scroll.y > TEXT_SIZE)
        m_Scroll.y -= TEXT_SIZE;
    else
        m_Scroll.y = 0;

    m_ShouldUpdateScroll = true;
}

void TextBox::ScrollDown() noexcept {
    float limit = TEXT_SIZE * (GetLineCount() - 1);
    if (m_Scroll.y < limit)
        m_Scroll.y += TEXT_SIZE;
    else
        m_Scroll.y = limit;

    m_ShouldUpdateScroll = true;
}

void TextBox::EnsureCursorVisibility() noexcept {
    auto [cursorX, cursorY] = m_Cursor.GetPosition();
    auto [cursorWidth, cursorHeight] = m_Cursor.GetSize();
    auto [textBoxX, textBoxY] = m_Position;
    auto [textBoxWidth, textBoxHeight] = m_Size;
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
    if (cursorX - textBoxX < scrollX) {
        scrollX = cursorX - textBoxX;
    }
}

const std::vector<std::string>& TextBox::GetBuffer() const noexcept {
    return m_Buffer;
}

sf::Vector2f TextBox::GetScroll() const noexcept {
    return m_Scroll;
}

std::optional<std::string> TextBox::Line(size_t row) const noexcept {
    if (row >= m_Buffer.size())
        return std::nullopt;

    return m_Buffer.at(row);
}

CursorLocation TextBox::GetCursorLocation() const noexcept {
    return m_Cursor.Current();
}

size_t TextBox::GetLineCount() const noexcept {
    return m_Buffer.size();
}

std::optional<char> TextBox::GetCharAt(const CursorLocation& pos) const noexcept {
    auto [row, col] = pos;

    // Make sure the position is within bounds.
    // Check if the row isn't bigger than lineCount
    // and the column isn't bigger than the line's size.
    if (row >= GetLineCount() || col >= m_Buffer.at(row).size())
        return std::nullopt;

    return m_Buffer.at(row).at(col);
}

std::optional<char> TextBox::GetRightChar() const noexcept {
    return GetCharAt(GetCursorLocation());
}

std::optional<char> TextBox::GetLeftChar() const noexcept {
    return GetCharAt(m_Cursor.Prev());
}

void TextBox::Add(char c) noexcept {
    ClearSelection();

    m_ShouldUpdateString = true;
    auto [row, col] = GetCursorLocation();
    auto& line = m_Buffer[row];

    // Insert an implicit newline.
    if (c == '\n') {
        // 1. Split the line at the cursor's position. 
        // 3. Insert a new line and append the tail.
        // 4. Move to the start of this new line. 
        std::string tail = std::string(line.begin() + col, line.end());
        line.erase(line.begin() + col, line.end()); // Erase the tail, so it isn't duplicated.

        m_Buffer.insert(m_Buffer.begin() + row + 1, {tail});

        MoveTo({ row + 1, 0 });

        return;
    }

    // Insert a character normally, make sure it is valid.
    if (!std::isprint(c))
        return;

    line.insert(line.begin() + col, c);
    MoveRight();
}

void TextBox::Add(const std::string& str) noexcept {
    for(const char c : str)
        Add(c);
}

void TextBox::AddTab() noexcept {
    for(size_t i = 0; i < TAB_WIDTH; i++)
        Add(' ');
}

bool TextBox::Remove() noexcept {
    if(ClearSelection()) {
        return true;
    }

    // Do nothing if the cursor is on the first character.
    if (m_Cursor.OnFirstPos())
        return false;

    auto [row, col] = GetCursorLocation();

    // We're on the start of the line, delete the implicit new line. 
    if (m_Cursor.OnStartLine())
        return RemoveRange({ row - 1, m_Buffer[row - 1].size() }, { row, col });

    // Delete a character normally.
    // -1 because we're deleting the character left of the cursor. 
    return RemoveRange({ row, col - 1 }, { row, col });
}

bool TextBox::SkipRemove() noexcept {
    if(ClearSelection()) {
        return true;
    }

    // Save the current cursor position, skip to the left,
    // and delete all characters in between.
    CursorLocation initial = GetCursorLocation();
    return SkipLeft() && RemoveRange(GetCursorLocation(), initial);
}

bool TextBox::RemoveRange(CursorLocation begin, CursorLocation end) noexcept {
    // Ensure the range is actually valid. 
    if (begin > m_Cursor.MaxPos() ||
        end > m_Cursor.MaxPos() ||
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

    MoveTo(begin);

    m_ShouldUpdateString = true;
    return true;
}

bool TextBox::RemoveTab() noexcept {
    for (size_t i = 0; i < TAB_WIDTH; i++) {
        if (GetLeftChar() == ' ')
            Remove();
        else
            return i != 0;
    }

    return true;
}

bool TextBox::IsSelecting() const noexcept {
    return m_SelectPos != CursorLocation::NPos();
}

void TextBox::StartSelecting() noexcept {
    m_SelectPos = GetCursorLocation();
    m_ShouldUpdateView = true;
}

void TextBox::StopSelecting() noexcept {
    m_SelectPos = CursorLocation::NPos();
    m_ShouldUpdateView = true;
}

bool TextBox::ClearSelection() noexcept {
    if(!IsSelecting())
        return false;

    // TODO: I use this in 3 places, maybe make it a function?
    RemoveRange(std::min(m_SelectPos, GetCursorLocation()), std::max(m_SelectPos, GetCursorLocation()));
    StopSelecting();
    return true;
}

std::optional<std::string> TextBox::GetSelection() const noexcept {
    if(!IsSelecting())
        return std::nullopt;

    auto [beginRow, beginCol] = std::min(m_SelectPos, GetCursorLocation());
    auto [endRow, endCol] = std::max(m_SelectPos, GetCursorLocation());

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

void TextBox::SelectAll() noexcept {
    StopSelecting();
    MoveTop();
    StartSelecting();
    MoveBottom();
    m_ShouldUpdateView = true;
}

bool TextBox::MoveTo(CursorLocation pos) noexcept {
    m_ShouldUpdateView = true;

    // Cursor is already at the provided pos. 
    if (pos == GetCursorLocation())
        return false;

    auto [row, col] = pos;

    // Clamp in case of invalid pos.
    if (row >= m_Buffer.size()) {
        row = m_Buffer.back().size(); col = m_Buffer.size() - 1;
    }
    if (col > m_Buffer[row].size()) {
        col = m_Buffer[row].size();
    }

    return m_Cursor.MoveTo({ row, col });
}

bool TextBox::MoveUp() noexcept {
   return MoveTo(m_Cursor.Above());
}

bool TextBox::MoveDown() noexcept {
    return MoveTo(m_Cursor.Below());
}

bool TextBox::MoveLeft() noexcept {
    return MoveTo(m_Cursor.Prev());
}

bool TextBox::MoveRight() noexcept {
    return MoveTo(m_Cursor.Next());
}

void TextBox::MoveTop() noexcept {
    MoveTo(m_Cursor.MinPos());
}

void TextBox::MoveBottom() noexcept {
    MoveTo(m_Cursor.MaxPos());
}

void TextBox::MoveStart() noexcept {
    MoveTo(m_Cursor.StartLinePos());
}

void TextBox::MoveEnd() noexcept {
    MoveTo(m_Cursor.EndLinePos());
}

bool TextBox::SkipLeft() noexcept {
    if (m_Cursor.OnFirstPos())
        return false;

    // If we're at the start of the line, just move to the left. 
    if (m_Cursor.OnStartLine()) {
        MoveLeft(); return true;
    }

    char leftChar = GetLeftChar().value(); // Must have a value, as we're not on firstPos.

    // Get the closest chars of each class. 
    auto nextLeftSpace = FindFirstLeft(isspace);
    auto nextLeftAlnum = FindFirstLeft(isalnum);
    auto nextLeftPunct = FindFirstLeft(ispunct);

    // A char can be one of three classes.
    // 1. Whitespace: '\n', '\t', ' ', etc.
    // 2. Alphanumeric: 'A', '3', 'h', etc.
    // 3. Punctuation: '*', '.', '+', etc.
    // In any case, just skip to the closest character that isn't of the leftChar's class. 
    // If one was found beyond the confines of the line, skip to the startPos. 
    auto found =    (isspace(leftChar)) ? std::max({ nextLeftAlnum, nextLeftPunct, m_Cursor.StartLinePos() }) :
                    (isalnum(leftChar)) ? std::max({ nextLeftSpace, nextLeftPunct, m_Cursor.StartLinePos() }) :
                    (ispunct(leftChar)) ? std::max({ nextLeftSpace, nextLeftAlnum, m_Cursor.StartLinePos() }) :
                    m_Cursor.MinPos();

    return MoveTo(found);
}

bool TextBox::SkipRight() noexcept {
    // We cannot skip if we're at the end.
    if (m_Cursor.OnLastPos())
        return false;

    // If we're at the end of the line, just move to the right. 
    if (m_Cursor.OnEndLine()) {
        MoveRight(); return true;
    }

    char rightChar = GetRightChar().value(); // Must have a value, as we're not on lastPos. 

    // Get the closest chars of each class. 
    auto nextRightSpace = FindFirstRight(isspace);
    auto nextRightAlnum = FindFirstRight(isalnum);
    auto nextRightPunct = FindFirstRight(ispunct);

    // A char can be one of three classes.
    // 1. Whitespace: '\n', '\t', ' ', etc.
    // 2. Alphanumeric: 'A', '3', 'h', etc.
    // 3. Punctuation: '*', '.', '+', etc.
    // In any case, just skip to the closest character that isn't of the rightChar's class. 
    // If one was found beyond the confines of the line, skip to the endPos. 
    auto found = (isspace(rightChar)) ? std::min({ nextRightAlnum, nextRightPunct, m_Cursor.EndLinePos() }) :
                 (isalnum(rightChar)) ? std::min({ nextRightSpace, nextRightPunct, m_Cursor.EndLinePos() }) :
                 (ispunct(rightChar)) ? std::min({ nextRightSpace, nextRightAlnum, m_Cursor.EndLinePos() }) :
                 m_Cursor.MaxPos();

    return MoveTo(found);
}

CursorLocation TextBox::FindFirstLeft(const std::function<bool(char)>& pred) const {
    // Move one position back each iteration and check
    // if the character at that position matches the predicate. 
    for (CursorLocation current = m_Cursor.Prev();; current = m_Cursor.Prev(current)) {
        auto c = GetCharAt(current);

        if (!c.has_value()) // Sanity check. Make sure it has a value. 
            continue;

        if (pred(c.value())) // +1, because we want to move to the *right* of the found character. 
            return current + CursorLocation(0, 1);

        if (current == m_Cursor.MinPos())
            break;
    }

    return m_Cursor.MinPos();
}

CursorLocation TextBox::FindFirstRight(const std::function<bool(char)>& pred) const {
    // Move one position forward each iteration and check
    // if the character at that position matches the predicate. 
    for (CursorLocation current = m_Cursor.Next(); current < m_Cursor.MaxPos(); current = m_Cursor.Next(current)) {
        auto c = GetCharAt(current);

        if (!c.has_value()) // Sanity check. Make sure it has a value. 
            continue;

        if (pred(c.value()))
            return current;
    }

    return m_Cursor.MaxPos();
}

void TextBox::Paste() noexcept {
    Add(sf::Clipboard::getString());
}

void TextBox::Copy() const noexcept {
    auto selection = GetSelection();

    if(selection.has_value())
        sf::Clipboard::setString(static_cast<std::string>(selection.value()));
}
