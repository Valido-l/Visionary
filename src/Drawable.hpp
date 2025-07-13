#pragma once

#include <SFML/Graphics.hpp>

class Drawable {
public:
    virtual void Draw(sf::RenderWindow& window) const = 0;
    virtual void Update(double deltaTime) = 0;

    sf::Vector2f GetPosition() const noexcept {
        return m_Position;
    }
    sf::Vector2f GetSize() const noexcept {
        return m_Size;
    }

    void SetPosition(sf::Vector2f pos) noexcept {
        m_Position = pos; OnTransformChanged();
    }
    void SetSize(sf::Vector2f size) noexcept {
        m_Size = size; OnTransformChanged();
    }

protected:
    virtual void OnTransformChanged() {}
    sf::Vector2f m_Position{}, m_Size{};
};