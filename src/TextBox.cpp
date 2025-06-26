#include "TextBox.h"
#include <iostream>

constexpr uint64_t TEXT_SIZE = 30;
constexpr uint64_t TAB_WIDTH = 4;

TextBox::TextBox(sf::Vector2f pos, sf::Vector2f size) : 
                    m_Index(0), m_SelectIndex(std::string::npos), m_String("Hell\no, World!"),
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

    sf::View textBoxView(m_Size / 2.0f + m_Scroll, m_Size);
    textBoxView.setViewport({{m_Position.x / windowSize.x, m_Position.y / windowSize.y}, {m_Size.x / windowSize.x, m_Size.y / windowSize.y}});

    window.setView(textBoxView);

    window.draw(m_Background);
    m_Text.Draw(window); 
    m_Cursor.Draw(window);
    window.draw(m_LineHighlight);

    window.setView(oldView); 
}

void TextBox::Update(double deltaTime) noexcept {
    m_Text.SetString(m_String);
    
    m_Cursor.Update(deltaTime);
    m_Text.Update(deltaTime); 
    
    UpdateString();
    UpdateView();
}

void TextBox::ClampCursor() noexcept {
    if(m_Index > m_String.size())
        m_Index = m_String.size();
}

void TextBox::OnTransformChanged() {
    m_Text.SetPosition(m_Position);
    m_Background.setPosition(m_Position + m_Scroll);

    m_Background.setSize(m_Size);
    m_LineHighlight.setSize({m_Size.x, m_LineHighlight.getSize().y});
}

void TextBox::UpdateString() {
    if(!m_ShouldUpdateString)
        return;

    m_Text.SetString(m_String); m_ShouldUpdateString = false;
}

void TextBox::UpdateView() {
    if(!m_ShouldUpdateView)
        return;
    
    m_Cursor.SetPosition(m_Text.FindCharacterPos(m_Index));
    m_LineHighlight.setPosition({m_Position.x + m_Scroll.x, m_Text.FindCharacterPos(m_Index).y});
    m_Background.setPosition(m_Position + m_Scroll);

    if(m_SelectIndex != std::string::npos)
        m_Text.Highlight(std::min(m_SelectIndex, m_Index), std::max(m_SelectIndex, m_Index));
    else
        m_Text.ClearHighlight();
    
    m_ShouldUpdateView = false;
}

size_t TextBox::GetCursorPos() const noexcept {
    return m_Index;
}

char TextBox::GetRightChar() const noexcept {
    if (m_Index == m_String.size())
        return '\0';

    return m_String.at(m_Index);
}

char TextBox::GetLeftChar() const noexcept {
    if (m_Index == 0)
        return '\0';

    return m_String.at(m_Index - 1);
}

void TextBox::Add(char c) noexcept {
    ClampCursor(); ClearSelection();

    m_String.insert(m_String.begin() + m_Index, c);
    MoveRight();

    m_ShouldUpdateString = true;
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
    if (m_Index == 0)
        return false;

    // -1 because we're deleting the character left of the cursor. 
    return RemoveRange(m_Index - 1, m_Index);
}

bool TextBox::SkipRemove() noexcept {
    if(IsSelecting()) {
        ClearSelection();
        return true;
    }

    // Save the current cursor position, skip to the left,
    // and delete all characters in between.
    size_t initialIndex = m_Index;
    return SkipLeft() && RemoveRange(m_Index, initialIndex);
}

bool TextBox::RemoveRange(size_t begin, size_t end) noexcept {
    ClampCursor(); 

    // Ensure the range is actually valid. 
    if (begin > end || begin > m_String.size() || end > m_String.size())
        return false;

    m_String.erase(m_String.begin() + begin, m_String.begin() + end);
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
    return m_SelectIndex != std::string::npos;
}

void TextBox::StartSelecting() noexcept {
    ClampCursor(); m_SelectIndex = m_Index;
    m_ShouldUpdateView = true;
}

void TextBox::StopSelecting() noexcept {
    m_SelectIndex = std::string::npos;
    m_ShouldUpdateView = true;
}

void TextBox::ClearSelection() noexcept {
    if(!IsSelecting())
        return;

    // TODO: I use this in 3 places, maybe make it a function?
    RemoveRange(std::min(m_SelectIndex, m_Index), std::max(m_SelectIndex, m_Index));
    StopSelecting();
}

// TODO: Use std::optional<std::string>.
std::string TextBox::GetSelection() const noexcept {
    if(!IsSelecting())
        return "";

    return m_String.substr(std::min(m_SelectIndex, m_Index), std::max(m_SelectIndex, m_Index));
}

bool TextBox::MoveTo(size_t index) noexcept {
    if (index > m_String.size())
        return false;

    m_Index = index; m_ShouldUpdateView = true;
    return true;
}

// TODO: Fix off-by-one bug when moving up to first line. 
bool TextBox::MoveUp() noexcept {
    // If we're on the first line, we can't move up.
    if (OnFirstLine())
        return false;

    // Get the distance of the lower line and set the cursor
    // to the beginning of the line.
    size_t lowerLineStartDistance = GetDistanceFromLineStart();
    MoveTo(m_Index - lowerLineStartDistance);

    std::cout << m_Index << std::endl;

    // Do the same for the upper line.
    size_t upperLineStartDistance = GetDistanceFromLineStart();
    MoveTo(m_Index - upperLineStartDistance);

    // Now the cursor is at the start of the second (upper) line.
    // If lowerLineStartDistance > upperLineStartDistance, we don't want to
    // overshoot and add too much, and if upperLineStartDistance > lowerLineStartDistance,
    // we want to keep the same relative row. 
    MoveTo(m_Index + std::min(lowerLineStartDistance, upperLineStartDistance));

    return true;
}

bool TextBox::MoveDown() noexcept {
    // If we're on the last line, we can't move down.
    if (OnLastLine())
        return false;

    // Get the distance from the start of the line.
    size_t firstLineStartDistance = GetDistanceFromLineStart();

    // Move the cursor to the end of the line. 
    MoveEnd();

    // Now, get the distance to the end of the line.
    // As we are at the start, this gets the characters in the line.
    size_t secondLineEndDistance = GetDistanceToLineEnd();

    // Add whatever is smaller to the cursor.
    // Remember, the cursor is at the start of the line,
    // if firstLineStartDistance > secondLineEndDistance, we don't want to
    // overshoot and add too much, and if secondLineEndDistance > firstLineStartDistance,
    // we want to keep the same relative row. 
    MoveTo(m_Index + std::min(firstLineStartDistance, secondLineEndDistance));

    return true;
}

bool TextBox::MoveLeft() noexcept {
    if (m_Index == 0)
        return false;

    MoveTo(m_Index - 1);
    return true;
}

bool TextBox::MoveRight() noexcept {
    if (m_Index >= m_String.size())
        return false;

    MoveTo(m_Index + 1);
    return true;
}

void TextBox::MoveTop() noexcept {
    MoveTo(0);
}

void TextBox::MoveBottom() noexcept {
    MoveTo(m_String.size());
}

void TextBox::MoveStart() noexcept {
    char leftChar = GetLeftChar();
    if (leftChar == '\n' || leftChar == '\0')
        return;

    size_t nextLeftNewline = FindFirstLeft('\n');
    MoveTo((nextLeftNewline != std::string::npos) ? nextLeftNewline + 1 : 0);
}

void TextBox::MoveEnd() noexcept {
    char rightChar = GetRightChar();
    if (rightChar == '\n' || rightChar == '\0')
        return; 

    size_t nextRightNewline = FindFirstRight('\n');
    MoveTo((nextRightNewline != std::string::npos) ? nextRightNewline : m_String.size());
}

bool TextBox::SkipLeft() noexcept {
    // We cannot skip if we're at the start.
    if (m_Index == 0)
        return false;

    char leftChar = GetLeftChar();

    // If we're on a new line, just move one to the right.
    if(leftChar == '\n') {
        MoveLeft(); return true;
    }

    // Get the closest chars of each class. 
    size_t nextLeftSpace = FindFirstLeft(isspace) + 1;
    size_t nextLeftAlnum = FindFirstLeft(isalnum) + 1;
    size_t nextLeftPunct = FindFirstLeft(ispunct) + 1;
    // +1 because we want the found character to be to the *left*, not right.

    // A char can be one of three classes.
    // 1. Whitespace: '\n', '\t', ' ', etc.
    // 2. Alphanumeric: 'A', '3', 'h', etc.
    // 3. Punctuation: '*', '.', '+', etc.
    // In any case, just skip to the closest character that isn't of the rightChar's class. 
    size_t foundIndex = (isspace(leftChar)) ? std::max(nextLeftAlnum, nextLeftPunct) :
                        (isalnum(leftChar)) ? std::max(nextLeftSpace, nextLeftPunct) :
                        (ispunct(leftChar)) ? std::max(nextLeftSpace, nextLeftAlnum) :
                                               std::string::npos;

    if (foundIndex != std::string::npos) {
        MoveTo(foundIndex);

        // If we skipped and landed on the start of a new line,
        // compensate by moving one to the right. 
        if(GetRightChar() == '\n')
            MoveRight();
    }
    else
        MoveTop(); // We're on the first line, move to the start.

    return true;
}

bool TextBox::SkipRight() noexcept {
    // We cannot skip if we're at the end.
    if (m_Index == m_String.size())
       return false;

    char rightChar = GetRightChar();

    // If we're on a new line, just move one to the right.
    if(rightChar == '\n') {
        MoveRight(); return true;
    }

    // Get the closest chars of each class. 
    size_t nextRightSpace = FindFirstRight(isspace);
    size_t nextRightAlnum = FindFirstRight(isalnum);
    size_t nextRightPunct = FindFirstRight(ispunct);

    // A char can be one of three classes.
    // 1. Whitespace: '\n', '\t', ' ', etc.
    // 2. Alphanumeric: 'A', '3', 'h', etc.
    // 3. Punctuation: '*', '.', '+', etc.
    // In any case, just skip to the closest character that isn't of the rightChar's class. 
    size_t foundIndex = (isspace(rightChar)) ? std::min(nextRightAlnum, nextRightPunct) :
                        (isalnum(rightChar)) ? std::min(nextRightSpace, nextRightPunct) :
                        (ispunct(rightChar)) ? std::min(nextRightSpace, nextRightAlnum) :
                                               std::string::npos;

    if (foundIndex != std::string::npos) {
        MoveTo(foundIndex);

        // If we skipped and landed on the start of a new line,
        // compensate by moving one to the left. 
        if(GetLeftChar() == '\n')
            MoveLeft();
    }
    else
        MoveBottom(); // We're on the last line, move to the end. 

    return true;
}

size_t TextBox::FindFirstLeft(const std::function<bool(char)>& pred) const {
    if (m_Index == 0 || m_Index > m_String.size())
        return std::string::npos;

    for (size_t i = m_Index - 1;; --i) {
        if (pred(m_String[i]))
            return i;
        if (i == 0) // Check the 0th index too.
            break;
    }

    return std::string::npos;
}

size_t TextBox::FindFirstRight(const std::function<bool(char)>& pred) const {
    if (m_Index >= m_String.size())
        return std::string::npos;

    for (size_t i = m_Index + 1; i < m_String.size(); i++) {
        if (pred(m_String[i]))
            return i;
    }

    return std::string::npos;
}

size_t TextBox::FindFirstLeft(char toFind) const {
    if (m_Index == 0 || m_Index > m_String.size())
        return std::string::npos;

    for (size_t i = m_Index - 1;; --i) {
        if (m_String[i] == toFind)
            return i;
        if (i == 0) // Check the 0th index too.
            break;
    }

    return std::string::npos;
}

size_t TextBox::FindFirstRight(char toFind) const {
    if (m_Index >= m_String.size())
        return std::string::npos;

    for (size_t i = m_Index + 1; i < m_String.size(); i++) {
        if (m_String[i] == toFind)
            return i;
    }

    return std::string::npos;
}

size_t TextBox::GetDistanceFromLineStart() const noexcept {
    size_t nextLeftNewline = FindFirstLeft('\n');
    return (nextLeftNewline == std::string::npos) ? m_Index : m_Index - nextLeftNewline;
}

size_t TextBox::GetDistanceToLineEnd() const noexcept {
    size_t nextRightNewline = FindFirstRight('\n');
    return (nextRightNewline == std::string::npos) ? m_String.size() - m_Index : nextRightNewline - m_Index;
}

bool TextBox::OnFirstLine() const noexcept {
    return FindFirstLeft('\n') == std::string::npos;
}

bool TextBox::OnLastLine() const noexcept {
    return GetRightChar() != '\n' && FindFirstRight('\n') == std::string::npos;
}

void TextBox::Paste() {
    Add(sf::Clipboard::getString());
}
