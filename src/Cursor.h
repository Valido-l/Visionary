#pragma once

#ifndef CURSOR_H
#define CURSOR_H

#include "Drawable.hpp"

class Cursor : public Drawable {
public:
    Cursor(sf::Vector2f size) : m_CursorShape(size) {
        SetPosition({0, 0}); SetSize(size);
        m_CursorShape.setFillColor(sf::Color::White);
    }

    void Draw(sf::RenderWindow& window) const override {
        window.draw(m_CursorShape);
    }   

    void Update(double deltaTime) override {}

    
private:
    void OnTransformChanged() override {
        m_CursorShape.setPosition(m_Position);
        m_CursorShape.setSize(m_Size);
    }
    
    sf::RectangleShape m_CursorShape;
};

#endif