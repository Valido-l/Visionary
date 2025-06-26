#pragma once

#ifndef TEXT_H
#define TEXT_H

#include <string>
#include <vector>

#include "Drawable.hpp"

using size_type = std::string::size_type;

class Text : public Drawable {
public:
    Text(const std::string& str = "", sf::Vector2f pos = {0, 0});

    void Draw(sf::RenderWindow& window) const override;
    void Update(double deltaTime) override;
    
    void SetString(const std::string& str);
    
    sf::Vector2f FindCharacterPos(size_t index);

    void ClearHighlight() noexcept;
    
    void Highlight(size_type begin, size_type end) noexcept;
private:

    void OnTransformChanged() override;
    
    size_type m_StringSize;
    sf::Text m_Text; 
    std::vector<sf::RectangleShape> m_Highlights;
};

#endif