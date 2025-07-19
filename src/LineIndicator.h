#pragma once

#include <vector>

#include "Drawable.hpp"
#include "Config.hpp"
#include "Theme.hpp"

class TextBox;

class LineIndicator : public Drawable, public Transformable, public Stylable<Theme::LineIndicatorTheme> {
public:
	LineIndicator(TextBox* owner, sf::Vector2f pos = { 0, 0 }, sf::Vector2f size = { 100, 0 }) noexcept;

	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

	void update(double deltaTime) override;

	void updateLines() noexcept;

private:
	void onTransformChanged(sf::Vector2f oldPos, sf::Vector2f oldSize) override;

	TextBox* m_Owner;
	sf::RectangleShape m_Background;
	std::vector<sf::Text> m_LineNumbers;
};