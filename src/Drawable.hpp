#pragma once

#include <SFML/Graphics.hpp>

/**
 * @brief   Slightly extended version of 'sf::Drawable', including an Update function. 
 */
class Drawable : public sf::Drawable {
public:
    virtual void update(double deltaTime) = 0;
    virtual ~Drawable() = default;
};

/**
 * @brief   Lightweight Transformable base-class, only including a position and a size. 
 */
class Transformable {
public:
    virtual ~Transformable() = default;

    sf::Vector2f getPosition() const noexcept {
        return m_Position;
    }
    sf::Vector2f getSize() const noexcept {
        return m_Size;
    }

    void setPosition(sf::Vector2f pos) noexcept {
        sf::Vector2f oldPos = m_Position;
        m_Position = pos; onTransformChanged(oldPos, m_Size);
    }
    void setSize(sf::Vector2f size) noexcept {
        sf::Vector2f oldSize = m_Size;
        m_Size = size; onTransformChanged(m_Position, oldSize);
    }

protected:    
    virtual void onTransformChanged(sf::Vector2f oldPos, sf::Vector2f oldSize) {}
    sf::Vector2f m_Position{}, m_Size{};
};