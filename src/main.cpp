#include <SFML/Graphics.hpp>
#include <cstdint>
#include <string>

#include "TextBox.h"

namespace Globals {
    uint32_t windowWidth = 800;
    uint32_t windowHeight = 600;
}

class TextEditor : public Drawable {
public:
    TextEditor(sf::Vector2f pos, sf::Vector2f size) : m_Lines({10, 10}) {
        SetPosition(pos); SetSize(size);
    }

    void Draw(sf::RenderWindow& window) const override {
        const auto oldView = window.getView();
        sf::Vector2u windowSize = window.getSize();    

        sf::View textEditorView(m_Size / 2.0f, m_Size);
        textEditorView.setViewport({{m_Position.x / windowSize.x, m_Position.y / windowSize.y}, {m_Size.x / windowSize.x, m_Size.y / windowSize.y}});
    
        window.setView(textEditorView);
        m_Lines.Draw(window);

        window.setView(oldView); 
    }

    void Update(double deltaTime) noexcept override {
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
            (!controlPressed) ? m_Lines.MoveStart() : m_Lines.MoveTop();
        if (key == sf::Keyboard::Key::End)
            (!controlPressed) ? m_Lines.MoveEnd() : m_Lines.MoveBottom();

        if (key == sf::Keyboard::Key::Up)
            m_Lines.MoveUp();
        if (key == sf::Keyboard::Key::Down)
            m_Lines.MoveDown();

        if(controlPressed && key == sf::Keyboard::Key::C)
            m_Lines.Copy();

        if(controlPressed && key == sf::Keyboard::Key::V)
            m_Lines.Paste();


        if(key == sf::Keyboard::Key::LShift) {
            (!m_Lines.IsSelecting()) ? m_Lines.StartSelecting() : m_Lines.StopSelecting();
        }

        if(key == sf::Keyboard::Key::Escape && m_Lines.IsSelecting())
            m_Lines.StopSelecting();
    }

    void OnTextEntered(const sf::Event::TextEntered& textEnteredEvent) noexcept {
        uint32_t unicode = textEnteredEvent.unicode;
		if (unicode >= 127 || unicode < 32)
			return;

        m_Lines.Add(static_cast<char>(unicode));
    }

private:
    void OnTransformChanged() override {
        m_Lines.SetSize(m_Size);
    }

    TextBox m_Lines;
};

int main()
{
    auto window = sf::RenderWindow(sf::VideoMode({Globals::windowWidth, Globals::windowHeight}), "Visionary");
    window.setFramerateLimit(144);

    TextEditor editor({0, 0}, {static_cast<float>(Globals::windowWidth), static_cast<float>(Globals::windowHeight)});
	sf::Clock deltaClock, clock; 

    const auto onClose = [&window](const sf::Event::Closed& closedEvent) {
        window.close();
    };

    const auto onResize = [&window, &editor](const sf::Event::Resized& resizedEvent) {
		Globals::windowWidth = resizedEvent.size.x; Globals::windowHeight = resizedEvent.size.y;
		auto size = sf::Vector2f(static_cast<float>(Globals::windowWidth),
                                 static_cast<float>(Globals::windowHeight));

        editor.SetSize(size);
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
  
        float fps = 1.0f / clock.restart().asSeconds();
		window.setTitle("Visionary | FPS: " + std::to_string(fps));


        editor.Update(deltaTime);
        
        window.clear();
        
        editor.Draw(window);

        window.display();
    }

    return 0;
}
