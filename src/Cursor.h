#pragma once

#include "CursorLocation.hpp"
#include "Drawable.hpp"

class TextBox;

/**
 * @brief   Class responsible for tracking and rendering the current cursor position inside a TextBox.
 */
class Cursor : public Drawable {
public:
    /**
     * @brief       Creates a cursor owned by a TextBox.
     *
     * @param owner The parent TextBox (can be nullptr).
     * @param size  Visual size of the cursor rectangle.
     * @param pos   Initial cursor location.
     */
    Cursor(TextBox* owner, sf::Vector2f size = { 0, 0 }, CursorLocation pos = { 0, 0 }) noexcept;

    /**
     * @brief   Draws the cursor to the provided render window.
     */
    virtual void Draw(sf::RenderWindow& window) const;

    virtual void Update(double deltaTime);

    /**
     * @brief       Moves the cursor to a provided location.
     *
     * @param to    The location to move to.
     * 
     * @return      True if the cursor was moved, false otherwise.
     */
    bool MoveTo(CursorLocation to);

    /**
     * @brief   Gets the current location of the cursor.
     */
    CursorLocation Current() const noexcept;
    /**
     * @brief   Location one character to the left of the cursor.
     *
     * @note    If the cursor is already at the start of the end,
     *          the same location is returned.
     */
    CursorLocation Prev() const noexcept;

    /**
     * @brief   Location one character to the left of @p pos.
     *
     * @note    When @p pos is at the start of the line, the result is the location
                of the last character of the previous line.
     * @note    If @p pos is already at the start of the buffer,
     *          the same location is returned.
     */
    CursorLocation Prev(CursorLocation pos) const noexcept;

    /**
     * @brief   Location one character to the right of the cursor.
     *
     * @note    If the cursor is already at the end of the buffer,
     *          the same location is returned.
     */
    CursorLocation Next() const noexcept;

    /**
    * @brief    Location one character to the right of @p pos.
    *
    * @note     When @p pos is at the end of the line, the result is the location
    *           of the first character of the next line.
    * @note     If @p pos is already at the end of the buffer,
    *           the same location is returned.
    */
    CursorLocation Next(CursorLocation pos) const noexcept;

    /**
     * @brief   Gets the location directly above the current one.
     *
     * @note    Returns the current location if already on the first line.
     */
    CursorLocation Above() const noexcept;
    /**
     * @brief   Gets the location directly below the current one.
     *
     * @note    Returns the current location if already on the last line.
     */
    CursorLocation Below() const noexcept;

    /**
     * @brief   Gets the first location of the cursor.
     *          By convention, this is { 0, 0 }.
     */
    CursorLocation MinPos() const noexcept;
    /**
     * @brief   Gets the last valid location in the buffer.
     */
    CursorLocation MaxPos() const noexcept;
    /**
     * @brief   Gets the first position of the current line.
     */
    CursorLocation StartLinePos() const noexcept;
    /**
     * @brief   Gets the position one past the last character of the current line.
     */
    CursorLocation EndLinePos() const noexcept;

    /**
     * @brief   Checks if the cursor is on the first line.
     */
    bool OnFirstLine() const noexcept;

    /**
     * @brief   Checks if the cursor is on the last line.
     */
    bool OnLastLine() const noexcept;

    /**
     * @brief   Checks if the cursor is at the start of the current line.
     */
    bool OnStartLine() const noexcept;

    /**
     * @brief   Checks if the cursor is at the end of the current line.
     */
    bool OnEndLine() const noexcept;

    /**
     * @brief   Checks if the cursor is at the very first buffer position.
     */
    bool OnFirstPos() const noexcept;

    /**
     * @brief   Checks if the cursor is at the very last buffer position.
     */
    bool OnLastPos() const noexcept;

    /**
     * @brief   Tests whether @p pos lies within the buffer bounds.
     */
    bool IsValidPos(CursorLocation pos) const noexcept;
private:
    /**
     * @brief   When called, updates the position and size of
     *          the cursor rectangle. 
     */
    virtual void OnTransformChanged();

    CursorLocation m_CursorLocation;
    sf::RectangleShape m_Shape;
    TextBox* m_Owner;
};