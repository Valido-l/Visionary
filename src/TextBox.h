#pragma once

#ifndef TEXT_BOX_H
#define TEXT_BOX_H

#include <optional>
#include <functional>

#include "Cursor.h"
#include "Text.h"

class TextBox : public Drawable {
public:
    TextBox(sf::Vector2f pos = {0, 0}, sf::Vector2f size = {0, 0});

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
    size_t GetCursorPos() const noexcept;

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
     * 
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
     * @note    It is required that 0 <= begin < end <= m_Text.size().
     * 
     * @param   begin The begin index.
     * @param   end   The end index.
     * 
     * @returns True if successful, false if 0 <= begin < end <= m_Text.size() isn't upheld. 
     */
    bool RemoveRange(size_t beginRow, size_t beginCol, size_t endRow, size_t endCol) noexcept;

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
     * @brief   Get the currently selected text.
     * 
     * @returns The currently selected text, or 'std::nullopt'
     *          if nothing is selected.
     */
    std::optional<std::string> GetSelection() const noexcept;

    // TODO: Add Select All and Copy functions. 

    /**
     * @brief   Moves the cursor to an index.
     * 
     * @param   index The index to move to.
     * 
     * @returns True if the move was successful, false if the index was an invalid position.
     */
    bool MoveTo(size_t row, size_t col) noexcept;

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
     * @brief   Clears the selected text.
     */
    void ClearSelection() noexcept;

    /**
     * @brief       Scan left from the current cursor position (exclusive) until a character
     *              satisfies the supplied predicate.
     *
     * @param pred  Unary predicate 'bool(char)' that returns 'true'
     *              for a matching character.
     * @return      Index of the first matching character, or 'std::string::npos'
     *              if none is found.
     */
    size_t FindFirstLeft(const std::function<bool(char)>& pred) const;

    /**
     * @brief       Scan right from the current cursor position (exclusive) until a character
     *              satisfies the supplied predicate.
     *
     * @param pred  Unary predicate 'bool(char)' that returns 'true'
     *              for a matching character.
     * @return      Index of the first matching character, or 'std::string::npos'
     *              if none is found.
     */
    size_t FindFirstRight(const std::function<bool(char)>& pred) const;

    /**
     * Checks if the cursor is on the first line.
     *
     * @returns True if the cursor is on the first line, false otherwise. 
     */
    bool OnFirstLine() const noexcept;

    /**
     * Checks if the cursor is on the last line.
     *
     * @returns True if the cursor is on the last line, false otherwise.
     */
    bool OnLastLine() const noexcept;

    bool OnStartLine() const noexcept;

    bool OnEndLine() const noexcept;

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
     */
    void UpdateView(); 

    /**
     * @brief Called whenever m_Position or m_Size change.
     */
    void OnTransformChanged() override;

    Cursor m_Cursor; Text m_Text;

    sf::RectangleShape m_Background, m_LineHighlight;
    // The current position of the cursor. 
    size_t m_Row, m_Col;
    //size_t m_Index; 
    //size_t m_SelectIndex; // The position of the cursor when selection was started. No selection is indicated by 'std::string::npos'.
    sf::View m_View; // The view that displays the TextBox. 
    sf::Vector2f m_Scroll; // The scroll of the TextBox. 

    bool m_ShouldUpdateString, m_ShouldUpdateView; // When true, the string / view will be updated. 

    std::vector<std::string> m_Content;
    //std::string m_String; // Content of the TextBox. 
};


#endif