#pragma once

#include <optional>
#include <functional>

#include "Cursor.h"
#include "Text.h"

/**
 *  @brief  Class that implements various ways of manipulating
 *          and moving through a buffer.
 *
 *          Also handles drawing the contents of the buffer, among
 *          various other things.
 */
class TextBox : public Drawable {
public:
    /**
     * @brief       Creates a TextBox object.
     *
     * @param pos   The position of the TextBox.
     * @param size  The size of the TextBox. 
     */
    TextBox(sf::Vector2f pos = { 0, 0 }, sf::Vector2f size = { 0, 0 });

    /**
     * @brief   Draw the elements of the TextBox.
     *
     * @param   window The window to draw to.
     */
    void Draw(sf::RenderWindow& window) const override;

    /**
     * @brief   Update the elements of the TextBox.
     *
     * @param   deltaTime The time elapsed since the last frame.
     */
    void Update(double deltaTime) noexcept override;

    /**
     * @returns The position of the cursor.
     */
    CursorLocation GetCursorLocation() const noexcept;

    /**
     * @returns The position of the cursor.
     */
    sf::Vector2f GetScroll() const noexcept;

    /**
     * @brief       Get the buffer of the TextBox.
     *
     * @returns     A const-reference to the m_Buffer vector.
     */
    const std::vector<std::string>& GetBuffer() const noexcept;

    /**
     * @brief       Get a line at a specific row.
     * 
     * @param row   The row. 
     * 
     * @returns     A const-string-reference to the line
     *              or 'std::nullopt' if the provided row
     *              is out of range. 
     */
    std::optional<std::string> Line(size_t row) const noexcept;

    /**
     * @brief   Get the amount of lines.
     */
    size_t GetLineCount() const noexcept;

    /**
     * @brief   Get a character at a specific position, even if selecting.
     *
     * @param   pos The position of the character. 
     * 
     * @returns The character at the provided position cursor 
                or 'std::nullopt' if the position is invalid. 
     */
    std::optional<char> GetCharAt(const CursorLocation& pos) const noexcept;

    /**
     * @brief   Get the character to the right of the cursor, even if selecting.
     *
     * @returns The character to the right of the cursor or 'std::nullopt'
     *          if there is nothing to the right.
     */
    std::optional<char> GetRightChar() const noexcept;

    /**
     * @brief   Get the character to the left of the cursor, even if selecting.
     *
     * @returns The character to the left of the cursor or 'std::nullopt'
     *          if there is nothing to the left.
     */
    std::optional<char>GetLeftChar() const noexcept;

    /**
     * @brief   Adds a character to the right of the cursor.
     *
     * @note    If selecting, the selected text is deleted.
     *
     * @param   c The character to add.
     */
    void Add(char c) noexcept;

    /**
     * @brief   Adds a string to the right of the cursor.
     *
     * @note    If selecting, the selected text is deleted.
     *
     * @param   str The string to add.
     */
    void Add(const std::string& str) noexcept;

    /**
     * @brief   Adds a tab where the cursor is, by inserting spaces.
     *
     * @note    The number of spaces depends on the defined tab width.
     * @note    Clears selection.
     */
    void AddTab() noexcept;

    /**
     * @brief   Removes a character to the left of the cursor.
     *
     * @note    If selecting, deletes the selected text.
     *
     * @returns True if the removal was successful, false otherwise.
     */
    bool Remove() noexcept;

    /**
     * @brief   If the character left of the cursor is alphanumeric,
     *          removes a sequence of alphanumeric characters, until a non-alphanumeric character is hit.
     *          Vice-versa for when a non-alphanumeric character is left of the cursor.
     *
     * @returns True if the SkipRemove was successful, false otherwise.
     */
    bool SkipRemove() noexcept;

    /**
     * @brief   Removes all characters in a range.
     *
     * @note    Sets the position of the cursor to begin after removing.
     * @note    It is required that minPos <= begin < end <= maxPos.
     *
     * @param   begin The begin position.
     * @param   end   The end position.
     *
     * @returns True if successful, false if minPos <= begin < end <= maxPos isn't upheld.
     */
    bool RemoveRange(CursorLocation begin, CursorLocation end) noexcept;

    /**
     * @brief   Removes a tab, by removing leading spaces.
     *
     * @note    The number of spaces removed depends on the defined tab width.
     *
     * @returns True if at least one space was removed, false otherwise.
     */
    bool RemoveTab() noexcept;

    /**
     * @brief   Check if text is currently being selected.
     *
     * @returns True if selecting, false otherwise.
     */
    bool IsSelecting() const noexcept;

    /**
     * @brief   Starts selecting from the current position of the cursor.
     */
    void StartSelecting() noexcept;

    /**
     * @brief   Stops selecting entirely.
     */
    void StopSelecting() noexcept;

    /**
     * @brief   Selects the entire contents of the TextBox.
     *
     * @note    Functions identically, even if text is already selected..
     */
    void SelectAll() noexcept;

    /**
     * @brief   Get the currently selected text.
     *
     * @returns The currently selected text, or 'std::nullopt'
     *          if nothing is selected.
     */
    std::optional<std::string> GetSelection() const noexcept;

    /**
     * @brief   Moves the cursor to a position.
     * 
     * @note    If the position is invalid, m_Row and m_Col
     *          will be clamped to the last position.
     *
     * @param   row The row to move to.
     * @param   col The column to move to.
     *
     * @returns True if the move was successful, false if the position was invalid.
     */
    bool MoveTo(CursorLocation pos) noexcept;

    /**
     * @brief   Tries to move the cursor up.
     *
     * @returns True if the cursor was moved up, false if it cannot be moved up.
     */
    bool MoveUp() noexcept;

    /**
     * @brief   Tries to move the cursor down.
     *
     * @returns True if the cursor was moved down, false if it cannot be moved down.
     */
    bool MoveDown() noexcept;

    /**
     * @brief   Tries to move the cursor left.
     *
     * @returns True if the cursor was moved left, false if it cannot be moved left.
     */
    bool MoveLeft() noexcept;

    /**
     * @brief   Tries to move the cursor right.
     *
     * @returns True if the cursor was moved right, false if it cannot be moved right.
     */
    bool MoveRight() noexcept;

    /**
     * @brief   Skips to the next-left character of a different class.
     *
     * @returns True if the skip was successful, false otherwise.
     */
    bool SkipLeft() noexcept;

    /**
     * @brief   Skips to the next-right character of a different class.
     *
     * @returns True if the skip was successful, false otherwise.
     */
    bool SkipRight() noexcept;

    /**
     * @brief   Moves the cursor to the very start of the text.
     */
    void MoveTop() noexcept;

    /**
     * @brief   Moves the cursor to the very end of the text.
     */
    void MoveBottom() noexcept;

    /**
     * @brief   Moves the cursor to the start of the line.
     */
    void MoveStart() noexcept;

    /**
     * @brief   Moves the cursor to the end of the line.
     */
    void MoveEnd() noexcept;

    /**
     * @brief   Moves the view up.
     */
    void ScrollUp() noexcept;

    /**
     * @brief   Moves the view down.
     */
    void ScrollDown() noexcept;

    /**
     * @brief   Pastes the contents of the clipboard where the cursor's current position is.
     *
     * @note    Removes selection.
     */
    void Paste() noexcept;

    /**
     * @brief   Sets the contents of the clipboard to the currently selected text.
     *
     * @note    Does nothing if no text is selected.
     */
    void Copy() const noexcept;

private:
    /**
     * @brief   Ensure the cursor is visible and 
     *
     * @note    Does nothing if no text is selected.
     */
    void EnsureCursorVisibility() noexcept;

    /**
     * @brief   Clears the selected text.
     * 
     * @returns True if anything was cleared at all.
     */
    bool ClearSelection() noexcept;

    /**
     * @brief       Scan left from the current cursor position (exclusive) until a character
     *              satisfies the supplied predicate.
     *
     * @param pred  Unary predicate 'bool(char)' that returns 'true'
     *              for a matching character.
     * @return      Index of the first matching character, or 'm_Cursor.MinPos()'
     *              if none is found.
     */
    CursorLocation FindFirstLeft(const std::function<bool(char)>& pred) const;

    /**
     * @brief       Scan right from the current cursor position (exclusive) until a character
     *              satisfies the supplied predicate.
     *
     * @param pred  Unary predicate 'bool(char)' that returns 'true'
     *              for a matching character.
     * @return      Index of the first matching character, or 'CursorPos::MaxPos()'
     *              if none is found.
     */
    CursorLocation FindFirstRight(const std::function<bool(char)>& pred) const;

    /**
     * @brief   If m_ShouldUpdateString is true, sets the string of m_Text.
     *
     * @note    Sets m_ShouldUpdateString to false after updating.
     */
    void UpdateString();

    /**
     * @brief   If m_ShouldUpdateView is true, updates the position
     *          of various elements in the TextBox.
     *
     * @note    Sets m_ShouldUpdateView to false after updating.
     * @note    Should be called before UpdateScroll.
     */
    void UpdateView();

    /**
     * @brief   If m_ShouldUpdateScroll is true, updates the scroll
     *          of various elements in the TextBox.
     *
     * @note    Sets m_ShouldUpdateScroll to false after updating.
     * @note    Should be called after UpdateView.
     */
    void UpdateScroll();

    /**
     * @brief Called whenever m_Position or m_Size change.
     *        Sets the size and position of various elements.   
     */
    void OnTransformChanged() override;

    Cursor m_Cursor; // The TextBox's cursor. Also manages the position of the cursor, in terms of rows and columns. 
    Text m_Text;

    sf::RectangleShape m_Background, m_LineHighlight;
    CursorLocation m_SelectPos; // The position of the cursor when selection was started. No selection is indicated by CursorLocation::NPos().
    sf::View m_View; // The view that displays the TextBox. 
    sf::Vector2f m_Scroll; // The scroll of the TextBox. 

    // When true, the string, view or scroll will be updated. 
    bool m_ShouldUpdateString, m_ShouldUpdateView, m_ShouldUpdateScroll; 

    std::vector<std::string> m_Buffer; // Buffer of the TextBox. 
};