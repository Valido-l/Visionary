#include "TextBox.h"

constexpr uint64_t TEXT_SIZE = 30;
constexpr uint64_t TAB_WIDTH = 4;

// Helper. Used to get the default text for drawing. 
static sf::Text& GetTextBase() {
    static bool firstCall = true;
    static sf::Font font;

    if (firstCall && !font.openFromFile("Fonts/anon.ttf"))
        throw std::runtime_error("Cannot load the font.");

    static sf::Text text(font, "", TEXT_SIZE);

    if (firstCall) {
        text.setFillColor(sf::Color::White);
        text.setPosition({ 0, 0 });
        firstCall = false;
    }

    return text;
}

TextBox::TextBox() : m_Index(0), m_Text("Hello.World"),
                     m_CursorShape({ 2.0f, TEXT_SIZE }), m_LineHighlight({800.0f, TEXT_SIZE}) {
    m_CursorShape.setFillColor(sf::Color::White);
    m_LineHighlight.setOutlineColor(sf::Color(90, 90, 90, 100));
    m_LineHighlight.setFillColor(sf::Color(50, 50, 50, 100));
    m_LineHighlight.setOutlineThickness(1.0f);
}

void TextBox::Draw(sf::RenderWindow& window) noexcept {
    sf::Text text(GetTextBase()); text.setString(m_Text);
    m_CursorShape.setPosition(text.findCharacterPos(m_Index));

    m_LineHighlight.setPosition({1, text.findCharacterPos(m_Index).y});

    window.draw(text);
    window.draw(m_CursorShape);
    window.draw(m_LineHighlight);
}

void TextBox::Update(double deltaTime) noexcept {

}

size_t TextBox::GetCursorPos() const noexcept {
    return m_Index;
}

char TextBox::GetRightChar() const {
    if (m_Index > m_Text.size())
        throw std::out_of_range("Tried accessing a character out of bounds");

    if (m_Index == m_Text.size())
        return '\0';

    return m_Text.at(m_Index);
}

char TextBox::GetLeftChar() const {
    if (m_Index > m_Text.size())
        throw std::out_of_range("Tried accessing a character out of bounds");

    if (m_Index == 0)
        return '\0';

    return m_Text.at(m_Index - 1);
}

void TextBox::Add(char c) {
    if (m_Text.size() < m_Index)
        throw std::out_of_range("Tried adding out of range.");
    m_Text.insert(m_Text.begin() + m_Index, c);
    MoveRight();
}

void TextBox::Add(const std::string& str) {
    if (m_Text.size() < m_Index)
    throw std::out_of_range("Tried adding out of range.");
    m_Text.insert(m_Index, str);
    MoveTo(m_Index + str.size());
}

bool TextBox::Remove() {
    // Do nothing if the cursor is on the first character.
    if (m_Index == 0)
        return false;

    if (m_Index > m_Text.size())
        throw std::out_of_range("Tried removing out of range.");

    m_Text.erase(m_Text.begin() + m_Index - 1); // -1 because we're deleting the character left of the cursor. 
    return MoveLeft() || true;
}

bool TextBox::SkipRemove() {
    // Save the current cursor position, skip to the left,
    // and delete all characters in between.
    size_t initialIndex = m_Index;
    return SkipLeft() && RemoveRange(m_Index, initialIndex);
}

bool TextBox::RemoveRange(size_t begin, size_t end) noexcept {
    // Ensure the range is actually valid. 
    if (begin > end || begin > m_Text.size() || end > m_Text.size())
        return false;

    m_Text.erase(m_Text.begin() + begin, m_Text.begin() + end);
    m_Index = begin;

    return true;
}

void TextBox::AddTab() {
    for(size_t i = 0; i < TAB_WIDTH; i++)
        Add(' ');
}

bool TextBox::RemoveTab() {
    for (size_t i = 0; i < TAB_WIDTH; i++) {
        if (GetLeftChar() == ' ')
            Remove();
        else
            return i != 0;
    }

    return true;
}

bool TextBox::MoveTo(size_t index) {
    if (index >= m_Text.size())
        return false;

    m_Index = index;
    return true;
}

bool TextBox::MoveUp() {
    // If we're on the first line, we can't move up.
    if (OnFirstLine())
        return false;

    // Get the distance of the lower line and set the cursor
    // to the beginning of the line.
    size_t lowerLineStartDistance = GetDistanceFromLineStart();
    m_Index -= lowerLineStartDistance;

    // Do the same for the upper line.
    size_t upperLineStartDistance = GetDistanceFromLineStart();
    m_Index -= upperLineStartDistance;

    // Now the cursor is at the start of the second (upper) line.
    // If lowerLineStartDistance > upperLineStartDistance, we don't want to
    // overshoot and add too much, and if upperLineStartDistance > lowerLineStartDistance,
    // we want to keep the same relative row. 
    m_Index += std::min(lowerLineStartDistance, upperLineStartDistance);

    return true;
}

bool TextBox::MoveDown() {
    // If we're on the last line, we can't move down.
    if (OnLastLine())
        return false;

    // Get the distance from the start of the line.
    size_t firstLineStartDistance = GetDistanceFromLineStart();

    // Move the cursor to the end of the line. 
    MoveEndLine();

    // Now, get the distance to the end of the line.
    // As we are at the start, this gets the characters in the line.
    size_t secondLineEndDistance = GetDistanceToLineEnd();

    // Add whatever is smaller to the cursor.
    // Remember, the cursor is at the start of the line,
    // if firstLineStartDistance > secondLineEndDistance, we don't want to
    // overshoot and add too much, and if secondLineEndDistance > firstLineStartDistance,
    // we want to keep the same relative row. 
    m_Index += std::min(firstLineStartDistance, secondLineEndDistance);

    return true;
}

bool TextBox::MoveLeft() {
    if (m_Index == 0)
        return false;

    m_Index--;
    return true;
}

bool TextBox::MoveRight() {
    if (m_Index >= m_Text.size())
        return false;

    m_Index++;
    return true;
}

void TextBox::MoveBegin() noexcept {
    m_Index = 0;
}

void TextBox::MoveEnd() noexcept {
    m_Index = m_Text.size();
}

void TextBox::MoveStartLine() noexcept {
    if (GetLeftChar() == '\n')
        return;

    size_t nextLeftNewline = FindFirstLeft('\n');
    m_Index = (nextLeftNewline != std::string::npos) ? nextLeftNewline + 1 : 0;
}

void TextBox::MoveEndLine() noexcept {
    if (GetRightChar() == '\n')
        return; 

    size_t nextRightNewline = FindFirstRight('\n');
    m_Index = (nextRightNewline != std::string::npos) ? nextRightNewline : m_Text.size();
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
        MoveBegin(); // We're on the first line, move to the start.

    return true;
}

bool TextBox::SkipRight() noexcept {
    // We cannot skip if we're at the end.
    if (m_Index == m_Text.size())
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
        MoveEnd(); // We're on the last line, move to the end. 

    return true;
}

size_t TextBox::FindFirstLeft(const std::function<bool(char)>& pred) const {
    if (m_Index == 0 || m_Index > m_Text.size())
        return std::string::npos;

    for (size_t i = m_Index - 1;; --i) {
        if (pred(m_Text[i]))
            return i;
        if (i == 0) // Check the 0th index too.
            break;
    }

    return std::string::npos;
}

size_t TextBox::FindFirstRight(const std::function<bool(char)>& pred) const {
    if (m_Index >= m_Text.size())
        return std::string::npos;

    for (size_t i = m_Index + 1; i < m_Text.size(); i++) {
        if (pred(m_Text[i]))
            return i;
    }

    return std::string::npos;
}

size_t TextBox::FindFirstLeft(char toFind) const {
    if (m_Index == 0 || m_Index > m_Text.size())
        return std::string::npos;

    for (size_t i = m_Index - 1;; --i) {
        if (m_Text[i] == toFind)
            return i;
        if (i == 0) // Check the 0th index too.
            break;
    }

    return std::string::npos;
}

size_t TextBox::FindFirstRight(char toFind) const {
    if (m_Index >= m_Text.size())
        return std::string::npos;

    for (size_t i = m_Index + 1; i < m_Text.size(); i++) {
        if (m_Text[i] == toFind)
            return i;
    }

    return std::string::npos;
}

size_t TextBox::GetDistanceFromLineStart() const noexcept {
    size_t nextLeftNewline = FindFirstLeft('\n');
    return (nextLeftNewline == std::string::npos) ? m_Index + 1 : m_Index - nextLeftNewline;
}

size_t TextBox::GetDistanceToLineEnd() const noexcept {
    size_t nextRightNewline = FindFirstRight('\n');
    return (nextRightNewline == std::string::npos) ? m_Text.size() - m_Index : nextRightNewline - m_Index;
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
