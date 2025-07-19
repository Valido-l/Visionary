#pragma once

#include "CursorLocation.hpp"
#include "Drawable.hpp"
#include "Config.hpp"
#include "Theme.hpp"

class TextBox;

/**
 * @brief   Class responsible for tracking and rendering the current cursor position inside a TextBox.
 */
class Cursor : public Drawable, public Transformable, public Stylable<Theme::CursorTheme> {
public:
    /**
     * @brief       Creates a cursor owned by a TextBox.
     *
     * @param owner The parent TextBox (can be nullptr).
     */
    Cursor(TextBox* owner) noexcept;

    /**
     * @brief   Draws the cursor to the provided render window.
     */
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

    virtual void update(double deltaTime) override;

    /**
     * @brief       Moves the cursor to a provided location.
     *
     * @param to    The location to move to.
     * 
     * @return      True if the cursor was moved, false otherwise.
     */
    bool moveTo(CursorLocation to);

    /**
     * @brief   Gets the current location of the cursor.
     */
    CursorLocation current() const noexcept;
    /**
     * @brief   Location one character to the left of the cursor.
     *
     * @note    If the cursor is already at the start of the end,
     *          the same location is returned.
     */
    CursorLocation prev() const noexcept;

    /**
     * @brief   Location one character to the left of @p pos.
     *
     * @note    When @p pos is at the start of the line, the result is the location
                of the last character of the previous line.
     * @note    If @p pos is already at the start of the buffer,
     *          the same location is returned.
     */
    CursorLocation prev(CursorLocation pos) const noexcept;

    /**
     * @brief   Location one character to the right of the cursor.
     *
     * @note    If the cursor is already at the end of the buffer,
     *          the same location is returned.
     */
    CursorLocation next() const noexcept;

    /**
    * @brief    Location one character to the right of @p pos.
    *
    * @note     When @p pos is at the end of the line, the result is the location
    *           of the first character of the next line.
    * @note     If @p pos is already at the end of the buffer,
    *           the same location is returned.
    */
    CursorLocation next(CursorLocation pos) const noexcept;

    /**
     * @brief   Gets the location directly above the current one.
     *
     * @note    Returns the current location if already on the first line.
     */
    CursorLocation above() const noexcept;
    /**
     * @brief   Gets the location directly below the current one.
     *
     * @note    Returns the current location if already on the last line.
     */
    CursorLocation below() const noexcept;

    /**
     * @brief   Gets the first location of the cursor.
     *          By convention, this is { 0, 0 }.
     */
    CursorLocation minPos() const noexcept;
    /**
     * @brief   Gets the last valid location in the buffer.
     */
    CursorLocation maxPos() const noexcept;
    /**
     * @brief   Gets the first position of the current line.
     */
    CursorLocation startLinePos() const noexcept;
    /**
     * @brief   Gets the position one past the last character of the current line.
     */
    CursorLocation endLinePos() const noexcept;

    /**
     * @brief   Checks if the cursor is on the first line.
     */
    bool onFirstLine() const noexcept;

    /**
     * @brief   Checks if the cursor is on the last line.
     */
    bool onLastLine() const noexcept;

    /**
     * @brief   Checks if the cursor is at the start of the current line.
     */
    bool onStartLine() const noexcept;

    /**
     * @brief   Checks if the cursor is at the end of the current line.
     */
    bool onEndLine() const noexcept;

    /**
     * @brief   Checks if the cursor is at the very first buffer position.
     */
    bool onFirstPos() const noexcept;

    /**
     * @brief   Checks if the cursor is at the very last buffer position.
     */
    bool onLastPos() const noexcept;

    /**
     * @brief   Tests whether @p pos lies within the buffer bounds.
     */
    bool isValidPos(CursorLocation pos) const noexcept;
private:
    /**
     * @brief   When called, updates the position and size of
     *          the cursor rectangle. 
     */
    virtual void onTransformChanged(sf::Vector2f oldPos, sf::Vector2f oldSize) override;

    CursorLocation m_CursorLocation;
    sf::RectangleShape m_Shape;
    TextBox* m_Owner;
};