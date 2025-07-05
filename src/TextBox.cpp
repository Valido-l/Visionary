#include "TextBox.h"

constexpr uint64_t TEXT_SIZE = 30;
constexpr uint64_t TAB_WIDTH = 4;

TextBox::TextBox(sf::Vector2f pos, sf::Vector2f size) : 
                    m_Row(0), m_Col(0), m_Content({"Hello, World!"}),
                    m_Cursor({ 2.0f, TEXT_SIZE }), m_Text(), 
                    m_Background(size), m_LineHighlight({size.x, TEXT_SIZE}),
                    m_Scroll(0.f, 0.f), m_ShouldUpdateString(true), m_ShouldUpdateView(true) {

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
    textBoxView.move(m_Position);

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
}

void TextBox::OnTransformChanged() {
    m_Text.SetPosition(m_Position + m_Scroll);
    m_Background.setPosition(m_Position);

    m_Background.setSize(m_Size);
    m_LineHighlight.setSize({m_Size.x, m_LineHighlight.getSize().y});
}

void TextBox::UpdateString() {
    if(!m_ShouldUpdateString)
        return;

    m_Text.SetString(m_Content); 
    m_ShouldUpdateString = false;
}

void TextBox::UpdateView() {
    if(!m_ShouldUpdateView)
        return;
    
    m_Text.SetPosition(m_Position + m_Scroll);

    sf::Vector2f newCursorPos = m_Text.FindCharacterPos(m_Row, m_Col);
    m_Cursor.SetPosition(newCursorPos);
    m_LineHighlight.setPosition({m_Position.x, newCursorPos.y});
    m_Background.setPosition(m_Position);

    //if(m_SelectIndex != std::string::npos)
    //    m_Text.Highlight(std::min(m_SelectIndex, m_Index), std::max(m_SelectIndex, m_Index));
    //else
    //    m_Text.ClearHighlight();
    
    m_ShouldUpdateView = false;
}

std::optional<char> TextBox::GetRightChar() const noexcept {
    // We're on the last position, nothing is to the right. 
    if (OnLastLine() && OnEndLine())
        return std::nullopt;

    return m_Content.at(m_Col).at(m_Row);
}

std::optional<char>  TextBox::GetLeftChar() const noexcept {
    // We're on the first position, nothing is to the left.
    if (OnFirstLine() && OnStartLine())
        return std::nullopt;

    // Return the last character of the previous line. 
    if (OnStartLine()) {
        return m_Content.at(m_Col - 1).back();
    }

    return m_Content.at(m_Col).at(m_Row);
}

void TextBox::Add(char c) noexcept {
    ClearSelection();

    m_ShouldUpdateString = true;
    auto& line = m_Content[m_Col];

    if (c == '\n') {
        std::string remainder = std::string(line.begin() + m_Row, line.end());
        line.erase(line.begin() + m_Row, line.end());

        m_Content.insert(m_Content.begin() + m_Col + 1, {remainder});

        MoveTo(0, m_Col + 1);

        return;
    }

    line.insert(line.begin() + m_Row, c);
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
    // TODO:    This is already being checked in ClearSelection.
    //          Maybe a better pattern?
    if(IsSelecting()) {
        ClearSelection();
        return true;
    }

    // Do nothing if the cursor is on the first character.
    if (OnFirstLine() && OnStartLine())
        return false;

    // We're on the start of the line, delete the implicit new line. 
    if (OnStartLine())
        return RemoveRange(m_Content[m_Col - 1].size(), m_Col - 1, m_Row, m_Col);

    // -1 because we're deleting the character left of the cursor. 
    return RemoveRange(m_Row - 1, m_Col, m_Row, m_Col);
}

bool TextBox::SkipRemove() noexcept {
    //if(IsSelecting()) {
    //    ClearSelection();
    //    return true;
    //}

    //// Save the current cursor position, skip to the left,
    //// and delete all characters in between.
    //size_t initialIndex = m_Index;
    //return SkipLeft() && RemoveRange(m_Index, initialIndex);
    return true;
}

bool TextBox::RemoveRange(size_t beginRow, size_t beginCol, size_t endRow, size_t endCol) noexcept {
    //ClampCursor(); 

    // Ensure the range is actually valid. 
    if  (beginCol > endCol                          ||
        (beginCol == endCol && beginRow >= endRow)  ||
        beginCol >= m_Content.size()                ||
        endCol >= m_Content.size()                  ||
        beginRow > m_Content[beginCol].size()      || 
        endRow > m_Content[endCol].size())
            return false;

    // First Case: On the same line.
    if (beginCol == endCol) {
        auto& line = m_Content[beginCol];
        line.erase(line.begin() + beginRow, line.begin() + endRow);
    }
    // Second Case: Different lines.
    else {
        // 1. Get the remainder of the endCol line.
        // 2. Delete everything between (beginRow, beginCol) and (endRow, endCol).
        // 3. Append the remainder to the beginCol line.

        auto& beginLine = m_Content[beginCol];
        auto& endLine = m_Content[endCol];

        std::string remainder = std::string(endLine.begin() + endRow, endLine.end());

        endLine.erase(endLine.begin() + endRow, endLine.end());
        beginLine.erase(beginLine.begin() + beginRow, beginLine.end());

        for (size_t i = beginCol + 1; i <= endCol; i++)
            m_Content.erase(m_Content.begin() + i);

        beginLine.append(remainder);
    }

    MoveTo(beginRow, beginCol);

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
    //return m_SelectIndex != std::string::npos;
    return false;
}

void TextBox::StartSelecting() noexcept {
    /*ClampCursor(); m_SelectIndex = m_Index;
    m_ShouldUpdateView = true;*/
}

void TextBox::StopSelecting() noexcept {
    //m_SelectIndex = std::string::npos;
    //m_ShouldUpdateView = true;
}

void TextBox::ClearSelection() noexcept {
    if(!IsSelecting())
        return;

    // TODO: I use this in 3 places, maybe make it a function?
    //RemoveRange(std::min(m_SelectIndex, m_Index), std::max(m_SelectIndex, m_Index));
    //StopSelecting();
}

// TODO: Use std::optional<std::string>.
std::optional<std::string> TextBox::GetSelection() const noexcept {
    //if(!IsSelecting())
    //    return std::nullopt;

    //return m_String.substr(std::min(m_SelectIndex, m_Index), std::max(m_SelectIndex, m_Index));
    return std::nullopt;
}

bool TextBox::MoveTo(size_t row, size_t col) noexcept {
    if (col >= m_Content.size() || row > m_Content[col].size())
        return false;

    m_Row = row; m_Col = col;
    m_ShouldUpdateView = true;
    return true;
}

// TODO: Fix off-by-one bug when moving up to first line. 
bool TextBox::MoveUp() noexcept {
    // If we're on the first line, we can't move up.
    if (OnFirstLine())
        return false;

   return MoveTo(std::min(m_Row, m_Content[m_Col - 1].size()), m_Col - 1);
}

bool TextBox::MoveDown() noexcept {
    // If we're on the last line, we can't move down.
    if (OnLastLine())
        return false;

    return MoveTo(std::min(m_Row, m_Content[m_Col + 1].size()), m_Col + 1);
}

bool TextBox::MoveLeft() noexcept {
    if (OnFirstLine() && OnStartLine())
        return false;

    if (OnStartLine())
        return MoveTo(m_Content[m_Col - 1].size(), m_Col - 1);

    return MoveTo(m_Row - 1, m_Col);
}

bool TextBox::MoveRight() noexcept {
    if (OnLastLine() && OnEndLine())
        return false;

    if (OnEndLine())
        return MoveTo(0, m_Col + 1);

    return MoveTo(m_Row + 1, m_Col);
}

void TextBox::MoveTop() noexcept {
    MoveTo(0, 0);
}

void TextBox::MoveBottom() noexcept {
    MoveTo(m_Content.back().size(), m_Content.size() - 1);
}

void TextBox::MoveStart() noexcept {
    MoveTo(0, m_Col);
}

void TextBox::MoveEnd() noexcept {
    MoveTo(m_Content[m_Col].size(), m_Col);
}

void TextBox::ScrollUp() noexcept {
    //if (m_Scroll.y >= 0)
    //    m_Scroll = {m_Scroll.x, 0};
    //else
    //    m_Scroll += {0, static_cast<float>(TEXT_SIZE)};

    //m_ShouldUpdateView = true;
}

void TextBox::ScrollDown() noexcept {
 /*   sf::Vector2f lastCharPos = m_Text.FindCharacterPos(std::string::npos);
    if (m_Scroll.y <= -m_Size.y - lastCharPos.y)
        m_Scroll = { m_Scroll.x, -m_Size.y - lastCharPos.y };
    else
        m_Scroll -= {0, static_cast<float>(TEXT_SIZE)};

    m_ShouldUpdateView = true;*/
}

bool TextBox::SkipLeft() noexcept {
    //// We cannot skip if we're at the start.
    //if (m_Index == 0)
    //    return false;

    //char leftChar = GetLeftChar();

    //// If we're on a new line, just move one to the right.
    //if(leftChar == '\n') {
    //    MoveLeft(); return true;
    //}

    //// Get the closest chars of each class. 
    //size_t nextLeftSpace = FindFirstLeft(isspace) + 1;
    //size_t nextLeftAlnum = FindFirstLeft(isalnum) + 1;
    //size_t nextLeftPunct = FindFirstLeft(ispunct) + 1;
    //// +1 because we want the found character to be to the *left*, not right.

    //// A char can be one of three classes.
    //// 1. Whitespace: '\n', '\t', ' ', etc.
    //// 2. Alphanumeric: 'A', '3', 'h', etc.
    //// 3. Punctuation: '*', '.', '+', etc.
    //// In any case, just skip to the closest character that isn't of the rightChar's class. 
    //size_t foundIndex = (isspace(leftChar)) ? std::max(nextLeftAlnum, nextLeftPunct) :
    //                    (isalnum(leftChar)) ? std::max(nextLeftSpace, nextLeftPunct) :
    //                    (ispunct(leftChar)) ? std::max(nextLeftSpace, nextLeftAlnum) :
    //                                           std::string::npos;
    //if (foundIndex != std::string::npos) {
    //    MoveTo(foundIndex);

    //    // If we skipped and landed on the start of a new line,
    //    // compensate by moving one to the right. 
    //    if(GetRightChar() == '\n')
    //        MoveRight();
    //}
    //else
    //    MoveTop(); // We're on the first line, move to the start.

    return true;
}

bool TextBox::SkipRight() noexcept {
    //// We cannot skip if we're at the end.
    //if (m_Index == m_String.size())
    //   return false;

    //char rightChar = GetRightChar();

    //// If we're on a new line, just move one to the right.
    //if(rightChar == '\n') {
    //    MoveRight(); return true;
    //}

    //// Get the closest chars of each class. 
    //size_t nextRightSpace = FindFirstRight(isspace);
    //size_t nextRightAlnum = FindFirstRight(isalnum);
    //size_t nextRightPunct = FindFirstRight(ispunct);

    //// A char can be one of three classes.
    //// 1. Whitespace: '\n', '\t', ' ', etc.
    //// 2. Alphanumeric: 'A', '3', 'h', etc.
    //// 3. Punctuation: '*', '.', '+', etc.
    //// In any case, just skip to the closest character that isn't of the rightChar's class. 
    //size_t foundIndex = (isspace(rightChar)) ? std::min(nextRightAlnum, nextRightPunct) :
    //                    (isalnum(rightChar)) ? std::min(nextRightSpace, nextRightPunct) :
    //                    (ispunct(rightChar)) ? std::min(nextRightSpace, nextRightAlnum) :
    //                                           std::string::npos;

    //if (foundIndex != std::string::npos) {
    //    MoveTo(foundIndex);

    //    // If we skipped and landed on the start of a new line,
    //    // compensate by moving one to the left. 
    //    if(GetLeftChar() == '\n')
    //        MoveLeft();
    //}
    //else
    //    MoveBottom(); // We're on the last line, move to the end. 

    return true;
}

size_t TextBox::FindFirstLeft(const std::function<bool(char)>& pred) const {
    //if (m_Index == 0 || m_Index > m_String.size())
    //    return std::string::npos;

    //for (size_t i = m_Index - 1;; --i) {
    //    if (pred(m_String[i]))
    //        return i;
    //    if (i == 0) // Check the 0th index too.
    //        break;
    //}

    return std::string::npos;
}

size_t TextBox::FindFirstRight(const std::function<bool(char)>& pred) const {
    //if (m_Index >= m_String.size())
    //    return std::string::npos;

    //for (size_t i = m_Index + 1; i < m_String.size(); i++) {
    //    if (pred(m_String[i]))
    //        return i;
    //}

    return std::string::npos;
}


bool TextBox::OnFirstLine() const noexcept {
    return m_Col == 0;
}

bool TextBox::OnLastLine() const noexcept {
    return m_Col == m_Content.size() - 1;
}

bool TextBox::OnStartLine() const noexcept {
    return m_Row == 0;
}

bool TextBox::OnEndLine() const noexcept {
    return m_Row == m_Content.at(m_Col).size();
}


void TextBox::Paste() noexcept {
    Add(sf::Clipboard::getString());
}

void TextBox::Copy() const noexcept {
    auto selection = GetSelection();

    if(selection.has_value())
        sf::Clipboard::setString(static_cast<std::string>(selection.value()));
}
