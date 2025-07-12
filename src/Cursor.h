#pragma once

#include "BufferPos.hpp"
#include "Drawable.hpp"

class TextBox;

class Cursor : public Drawable {
public:
    Cursor(TextBox* owner, sf::Vector2f size = {0, 0}, BufferPos pos = {0, 0}) noexcept;

    virtual void Draw(sf::RenderWindow& window) const;

    virtual void Update(double deltaTime);

    bool MoveTo(BufferPos to);

    /**
     * @brief   Gets the current position of the cursor.
     *
     * @return  The current BufferPos. 
     */
    BufferPos Current() const noexcept;
    /**
     * @brief   Gets the position of the cursor
     *          one character to the left.
     *
     * @note    Returns the current position if there
     *          are no characters to the left.
     *
     * @return  The previous BufferPos.
     */
    BufferPos Prev() const noexcept;

    /**
     * @brief   Gets the position of the cursor
     *          one character to the left.
     *
     * @note    Returns the current position if there
     *          are no characters to the left.
     *
     * @return  The previous BufferPos.
     */
    BufferPos Prev(BufferPos pos) const noexcept;

    /**
     * @brief   Gets the position of the cursor
     *          one character to the right.
     *
     * @note    Returns the current position if there
     *          are no characters to the right.
     *
     * @return  The next BufferPos.
     */
    BufferPos Next() const noexcept;

    /**
     * @brief   Gets the position of the cursor
     *          one character to the right.
     *
     * @note    Returns the current position if there
     *          are no characters to the right.
     *
     * @return  The next BufferPos.
     */
    BufferPos Next(BufferPos pos) const noexcept;

    BufferPos Above() const noexcept;

    BufferPos Below() const noexcept;

    BufferPos MinPos() const noexcept;
    BufferPos MaxPos() const noexcept;
    BufferPos StartLinePos() const noexcept;
    BufferPos EndLinePos() const noexcept;
    
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

    bool OnFirstPos() const noexcept;

    bool OnLastPos() const noexcept;

    bool IsValidPos(BufferPos pos) const noexcept;
private:

    virtual void OnTransformChanged();

    BufferPos m_BufferPos;
    sf::RectangleShape m_Shape;
    TextBox* m_Owner;
};