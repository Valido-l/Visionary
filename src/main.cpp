#include <SFML/Graphics.hpp>
#include <cstdint>
#include <string>

#include "TextBox.h"

namespace Globals {
    uint32_t windowWidth = 800;
    uint32_t windowHeight = 600;
}

class TextEditor {
public:
    TextEditor() : m_Lines() {}

    void Draw(sf::RenderWindow& window) noexcept {
        m_Lines.Draw(window);
    }

    void Update(double deltaTime) noexcept {
        m_Lines.Update(deltaTime);
    }

    void OnKeyPressed(const sf::Event::KeyPressed& keyPressedEvent) noexcept {
        auto key = keyPressedEvent.code;
		bool controlPressed = keyPressedEvent.control;
		bool shiftPressed = keyPressedEvent.shift;
        bool altPressed = keyPressedEvent.alt;

        if(key == sf::Keyboard::Key::Enter)
            m_Lines.Add('\n');

        if (key == sf::Keyboard::Key::Tab) {
            if (!shiftPressed) m_Lines.AddTab();
            else m_Lines.RemoveTab();
        }

        if (key == sf::Keyboard::Key::Backspace) {
            // If Ctrl is pressed, skip-remove.
            (!controlPressed) ? m_Lines.Remove() : m_Lines.SkipRemove();
        }

        if (key == sf::Keyboard::Key::Left) {
            // If control is not pressed move normally. Otherwise, skip-move. 
            (!controlPressed) ? m_Lines.MoveLeft() : m_Lines.SkipLeft();
        }

        if (key == sf::Keyboard::Key::Right) {
            (!controlPressed) ? m_Lines.MoveRight() : m_Lines.SkipRight();
        }

        if (key == sf::Keyboard::Key::Home)
            (!shiftPressed) ? m_Lines.MoveStartLine() : m_Lines.MoveBegin();
        if (key == sf::Keyboard::Key::End)
            (!shiftPressed) ? m_Lines.MoveEndLine() : m_Lines.MoveEnd();

        if (key == sf::Keyboard::Key::Up)
            m_Lines.MoveUp();
        if (key == sf::Keyboard::Key::Down)
            m_Lines.MoveDown();

        if(controlPressed && key == sf::Keyboard::Key::V)
            m_Lines.Paste();
    }

    void OnTextEntered(const sf::Event::TextEntered& textEnteredEvent) noexcept {
        uint32_t unicode = textEnteredEvent.unicode;
		if (unicode >= 127 || unicode < 32)
			return;

        m_Lines.Add(static_cast<char>(unicode));
    }

private:
    TextBox m_Lines;
};

int main()
{
    auto window = sf::RenderWindow(sf::VideoMode({800u, 600u}), "Visionary");
    window.setFramerateLimit(144);

    TextEditor editor;
	sf::Clock deltaClock, clock; 
	sf::View mainView(	{ static_cast<float>(Globals::windowWidth), static_cast<float>(Globals::windowHeight) },
						{ static_cast<float>(Globals::windowWidth), static_cast<float>(Globals::windowHeight) });
	
    const auto onClose = [&window](const sf::Event::Closed& closedEvent) {
        window.close();
    };

    const auto onResize = [&window, &mainView](const sf::Event::Resized& resizedEvent) {
		Globals::windowWidth = resizedEvent.size.x; Globals::windowHeight = resizedEvent.size.y;
		auto size = sf::Vector2f(static_cast<float>(Globals::windowWidth),
                                 static_cast<float>(Globals::windowHeight));
		mainView.setSize(size);
		mainView.setCenter(size / 2.0f);
	};

    const auto onKeyPressed = [&editor](const sf::Event::KeyPressed& keyPressedEvent) {
		editor.OnKeyPressed(keyPressedEvent);
    };

    const auto onTextEntered = [&editor](const sf::Event::TextEntered& textEnteredEvent) {
        editor.OnTextEntered(textEnteredEvent);
    };

    while (window.isOpen())
    {
        double deltaTime = deltaClock.restart().asSeconds();
        window.handleEvents(onClose, onResize, onKeyPressed, onTextEntered);

        window.setView(mainView);

        window.clear();
        editor.Draw(window);
        window.display();
    }

    return 0;
}
