#pragma once

#ifndef TEXT_H
#define TEXT_H

#include <unordered_map>
#include <string>
#include <vector>

#include "Drawable.hpp"

class Text : public Drawable {
public:
    Text(const std::vector<std::string>& strVec = { "" }, sf::Vector2f pos = { 0, 0 });

    void Draw(sf::RenderWindow& window) const override;
    void Update(double deltaTime) override;
    
    void SetString(const std::vector<std::string>& strVec);
    
    sf::Vector2f FindCharacterPos(size_t row, size_t col) const;

    void ClearHighlight() noexcept;
    
    void Highlight(size_t begin, size_t end) noexcept;
private:

    void OnTransformChanged() override;
    
    std::vector<sf::Text> m_Text; 
    std::unordered_map<size_t, sf::RectangleShape> m_HighlightCache;
};

#endif