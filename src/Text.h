#pragma once

#include <unordered_map>
#include <string>
#include <vector>

#include "CursorLocation.hpp"
#include "Drawable.hpp"
#include "Config.hpp"

class TextBox;

/**
 * @brief   Class responsible for drawing and highlighting parts of a TextBox's buffer. 
 */
class Text : public Drawable, public Transformable {
public:
    /**
     * @brief       Creates a cursor owned by a TextBox.
     *
     * @param owner The parent TextBox (can be nullptr).
     */
    Text(TextBox* owner);

    /**
     * @brief   Draw any text and highlights that have been created.
     */
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

    void update(double deltaTime) override;
    
    /**
     * @brief   When called, updates the text to be
     *          rendered, by sourcing it from m_Owner->GetBuffer().
     *          It then creates a new sf::Text object for each line
     *          and saves it to m_Text. 
     *          
     * @note    Does not create text objects that are out-of-frame.
     */
    void updateText();
    
    /**
     * @brief           Gets the position of a character at 
     *                  a specific location.
     *
     * @note            Can find the character pos, even if the
     *                  line is not being rendered. 
     *
     * @param   pos     The location of the character.
     * 
     * @returns         An 'sf::Vector2f' containing the position
     *                  of the character. 
     */
    sf::Vector2f findCharacterPos(CursorLocation pos) const;

    /**
     * @brief   Clears all highlights.
     */
    void clearHighlight() noexcept;
    
    /**
     * @brief           Highlights all text in a range by drawing 
     *                  sf::RectangleShapes below the highlighted text.
     *                  These RectangleShapes are stored in m_Highlights.
     *
     * @note            Does not create highlights that are out-of-frame.
     * @note            It is required that minPos <= begin < end <= maxPos.
     *
     * @param   begin   The begin position.
     * @param   end     The end position.
     */
    void highlight(CursorLocation begin, CursorLocation end) noexcept;
private:
    /**
     * @brief   When called, updates the position of all sf::Text objects in m_Text.               
     */
    void onTransformChanged(sf::Vector2f oldPos, sf::Vector2f oldSize) override;
    
    sf::Text buildText(const std::string& str, uint32_t fontSize, sf::Vector2f pos, const sf::Color& color) const noexcept;

    TextBox* m_Owner;
    std::vector<sf::Text> m_Text; 
    std::vector<sf::RectangleShape> m_Highlights;
};