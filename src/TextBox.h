#pragma once

#ifndef LINE_MANAGER_H
#define LINE_MANAGER_H

#include <functional>
#include <SFML/Graphics.hpp>
#include <string>

class TextBox {
public:
    TextBox();

    /**
     * Draw the elements of the LineManager.
     * 
     * @param window The window to draw to.
     */
    void Draw(sf::RenderWindow& window) noexcept;

    /**
     * Update the elements of the LineManager.
     *
     * @param deltaTime The time elapsed since the last frame. 
     */
    void Update(double deltaTime) noexcept;

    /**
     * @returns The position of the cursor. 
     */
    size_t GetCursorPos() const noexcept;

    /**
     * Get the character to the right of the cursor, even if selecting.
     *
     * @throws std::out_of_range Thrown if the cursor is out of bounds.
     *
     * @returns  The character to the right of the cursor or '\0'
     *           if there is nothing to the right.
     */
    char GetRightChar() const;

    /**
     * Get the character to the left of the cursor, even if selecting.
     *
     * @throws std::out_of_range Thrown if the cursor is out of bounds.
     *
     * @returns  The character to the left of the cursor or '\0'
     *           if there is nothing to the left.
     */
    char GetLeftChar() const;

    /**
     * Adds a character to the right of the cursor.
     * If selecting, the selected text is deleted.
     * 
     * @param c The character to add.
     * 
     * @throws std::out_of_range Thrown if the position of the cursor is out of bounds.
     */
    void Add(char c);

    /**
     * Adds a string to the right of the cursor.
     * If selecting, the selected text is deleted.
     * 
     * @param str The string to add.
     * 
     * @throws std::out_of_range Thrown if the position of the cursor is out of bounds.
     */
    void Add(const std::string& str);

    /**
     * Removes a character to the left of the cursor.
     * If selecting, deletes the selected text.
     * 
     * @throws std::out_of_range Thrown if the position of the cursor is out of bounds.
     * 
     * @returns True if the removal was successful, false otherwise. 
     */
    bool Remove();

    /**
     * If the character left of the cursor is alphanumeric, 
     * removes a sequence of alphanumeric characters, until a non-alphanumeric character is hit.
     * Vice-versa for when a non-alphanumeric character is left of the cursor.  
     *
     * @throws std::out_of_range Thrown if the position of the cursor is out of bounds.
     * 
     * @returns True if the SkipRemove was successful, false otherwise. 
     */
    bool SkipRemove(); 

    /**
     * Removes all characters in a range.
     * Sets the position of the cursor to begin after removing. 
     * @note It is required that begin < end < m_Text.size()
     * 
     * @param begin The begin index.
     * @param end   The end index.
     * 
     * @returns True if successful, false otherwise. 
     */
    bool RemoveRange(size_t begin, size_t end) noexcept;

    /**
     * Adds a tab where the cursor is, by inserting spaces. 
     * The number of spaces depends on the defined tab width.
     *
     * @throws std::out_of_range Thrown if the cursor is out of bounds.
     *
     * @note Clears selection. 
     */
    void AddTab();

    /**
     * Removes a tab, by removing leading spaces. 
     * The number of spaces removed depends on the defined tab width.
     * 
     * @returns True if at least one space was removed, false otherwise. 
     */
    bool RemoveTab();

    /**
     * Moves the cursor to an index.
     * 
     * @param index The index to move to.
     * 
     * @returns True if the move was successful, false otherwise.
     */
    bool MoveTo(size_t index);

    bool MoveUp();

    bool MoveDown();

    bool MoveLeft();

    bool MoveRight();

    /**
     * If the character to the left is alphanumeric, skips to the next-left non-alphanumeric character. 
     * If the character to the left is non-alphanumeric, skips to the next-left alphanumeric character. 
     *
     * @returns True if the skip was successful, false otherwise.
     */
    bool SkipLeft() noexcept;

    /**
     * If the character to the right is alphanumeric, skips to the next-right non-alphanumeric character. 
     * If the character to the right is non-alphanumeric, skips to the next-right alphanumeric character. 
     *
     * @returns True if the skip was successful, false otherwise.
     */
    bool SkipRight() noexcept;

    /**
     * Moves the cursor to the very start of the text.
     */
    void MoveBegin() noexcept;

    /**
     * Moves the cursor to the very end of the text.
     */
    void MoveEnd() noexcept;

    /**
     * Moves the cursor to the start of the line.
     */
    void MoveStartLine() noexcept;

    /**
     * Moves the cursor to the end of the line.
     */
    void MoveEndLine() noexcept;

    /**
     * Pastes the contents of the clipboard where the cursor's current position is.
     * 
     * @note Removes selection. 
     */
    void Paste();

private:
        /**
     * Scan left from the current cursor position (exclusive) until a character
     * satisfies the supplied predicate.
     *
     * @param pred  Unary predicate 'bool(char)' that returns 'true'
     *              for a matching character.
     * @return      Index of the first matching character, or 'std::string::npos'
     *              if none is found.
     */
    size_t FindFirstLeft(const std::function<bool(char)>& pred) const;

    /**
     * Finds the first instance of a character to the left of the cursor.
     *
     * @param c The character to find.
     *
     * @returns The index of the first instance,or std::string::npos
     *          if no instance was found.
     */
    size_t FindFirstLeft(char toFind) const;

    /**
     * Scan right from the current cursor position (exclusive) until a character
     * satisfies the supplied predicate.
     *
     * @param pred  Unary predicate 'bool(char)' that returns 'true'
     *              for a matching character.
     * @return      Index of the first matching character, or 'std::string::npos'
     *              if none is found.
     */
    size_t FindFirstRight(const std::function<bool(char)>& pred) const;

    /**
     * Finds the first instance of a character to the right of the cursor.
     *
     * @param c The character to find.
     *
     * @returns The index of the first instance,or std::string::npos
     *          if no instance was found.
     */
    size_t FindFirstRight(char toFind) const;

    /**
     * Gets the distance of the cursor from the line's start.
     *
     * @returns The distance of the cursor to the start of the line.
     */
    size_t GetDistanceFromLineStart() const noexcept;

    /**
     * Gets the distance of the cursor to the line's end.
     *
     * @returns The disttance of the cursor to the end of the line.
     */
    size_t GetDistanceToLineEnd() const noexcept;

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


    sf::RectangleShape m_CursorShape, m_LineHighlight;
    size_t m_Index; // The current position of the cursor.
    std::string m_Text;
};


#endif