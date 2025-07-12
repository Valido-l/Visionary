#pragma once

#include <unordered_map>
#include <string>
#include <vector>

#include "BufferPos.hpp"
#include "Drawable.hpp"

class Text : public Drawable {
public:
    Text(const std::vector<std::string>& strVec = { "" }, sf::Vector2f pos = { 0, 0 });

    void Draw(sf::RenderWindow& window) const override;
    void Update(double deltaTime) override;
    
    void SetString(const std::vector<std::string>& strVec);
    
    sf::Vector2f FindCharacterPos(BufferPos pos) const;

    void ClearHighlight() noexcept;
    
    void Highlight(BufferPos begin, BufferPos end) noexcept;
private:

    void OnTransformChanged() override;
    
    std::vector<sf::Text> m_Text; 
    std::vector<sf::RectangleShape> m_HighlightVec;
};