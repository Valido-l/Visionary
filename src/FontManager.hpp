#pragma once

#include "Theme.hpp"

class FontManager {
public:
	FontManager(const std::string& name) {
		if(!m_Font.openFromFile("Fonts/" + name))
			throw std::runtime_error("Cannot load the font.");
	}

	inline static sf::Font& getFont() {
		static FontManager instance(Theme::Get<Theme::AllThemes>().fontName);
		return instance.m_Font;
	}

private:
	sf::Font m_Font;
};