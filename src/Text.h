#pragma once

#ifndef TEXT_H
#define TEXT_H

#include <string>

#include "Drawable.hpp"

class Text : public Drawable {
public:
    Text(const std::string& str = "", sf::Vector2f pos = {0, 0});

    void Draw(sf::RenderWindow& window) const override;
    void Update(double deltaTime) override;
    
    void SetString(const std::string& str);
    
    sf::Vector2f FindCharacterPos(size_t index);
    
private:
    void OnTransformChanged() override;
    
    sf::Text m_Text;
};

#endif