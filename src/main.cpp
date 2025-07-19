#include <SFML/Graphics.hpp>
#include <cstdint>
#include <string>
#include <unordered_map>
#include <nlohmann/json.hpp>

#include "TextBox.h"

class TextEditor : public Drawable, public Transformable, public Stylable<Theme::TextEditorTheme> {
public:
    TextEditor(sf::Vector2f pos, sf::Vector2f size) : m_Lines()  {
        m_Lines.setPosition(m_Theme.offset);
        setPosition(pos); setSize(size);
    }

    void draw(sf::RenderTarget& target, sf::RenderStates states) const override {
        const auto oldView = target.getView();
        sf::Vector2u windowSize = target.getSize();
        sf::View textEditorView(m_Size / 2.0f, m_Size);
        textEditorView.setViewport({ 
            {m_Position.x / windowSize.x, m_Position.y / windowSize.y},
            {m_Size.x / windowSize.x, m_Size.y / windowSize.y} });

        target.setView(textEditorView);

        target.draw(m_Lines, states);

        target.setView(oldView);
    }

    void update(double deltaTime) noexcept override {
        m_Lines.update(deltaTime);
    }

    void onMouseWheelScroll(const sf::Event::MouseWheelScrolled mouseWheelEvent) noexcept {
        auto delta = mouseWheelEvent.delta;
        if (delta < 0)
            m_Lines.scrollDown();
        else
            m_Lines.scrollUp();
    }

    void onKeyPressed(const sf::Event::KeyPressed& keyPressedEvent) noexcept {
        auto key = keyPressedEvent.code;
		bool controlPressed = keyPressedEvent.control;
		bool shiftPressed = keyPressedEvent.shift;
        bool altPressed = keyPressedEvent.alt;

        if(key == sf::Keyboard::Key::Enter)
            m_Lines.add('\n');

        if (key == sf::Keyboard::Key::Tab) {
            if (!shiftPressed) m_Lines.addTab();
            else m_Lines.removeTab();
        }

        if (key == sf::Keyboard::Key::Backspace) {
            // If Ctrl is pressed, skip-remove.
            (!controlPressed) ? m_Lines.remove() : m_Lines.skipRemove();
        }

        if (key == sf::Keyboard::Key::Left) {
            // If control is not pressed move normally. Otherwise, skip-move. 
            (!controlPressed) ? m_Lines.moveLeft() : m_Lines.skipLeft();
        }

        if (key == sf::Keyboard::Key::Right) {
            (!controlPressed) ? m_Lines.moveRight() : m_Lines.skipRight();
        }

        if (key == sf::Keyboard::Key::Home)
            (!controlPressed) ? m_Lines.moveStart() : m_Lines.moveTop();
        if (key == sf::Keyboard::Key::End)
            (!controlPressed) ? m_Lines.moveEnd() : m_Lines.moveBottom();

        if (key == sf::Keyboard::Key::Up)
            m_Lines.moveUp();
        if (key == sf::Keyboard::Key::Down)
            m_Lines.moveDown();

        if (controlPressed && key == sf::Keyboard::Key::A)
            m_Lines.selectAll();

        if(controlPressed && key == sf::Keyboard::Key::C)
            m_Lines.copy();

        if(controlPressed && key == sf::Keyboard::Key::V)
            m_Lines.paste();

        if(key == sf::Keyboard::Key::LShift) {
            (!m_Lines.isSelecting()) ? m_Lines.startSelecting() : m_Lines.stopSelecting();
        }

        if(key == sf::Keyboard::Key::Escape && m_Lines.isSelecting())
            m_Lines.stopSelecting();
    }

    void onTextEntered(const sf::Event::TextEntered& textEnteredEvent) noexcept {
        uint32_t unicode = textEnteredEvent.unicode;
		if (unicode >= 127 || unicode < 32)
			return;

        m_Lines.add(static_cast<char>(unicode));
    }

private:
    void onTransformChanged(sf::Vector2f oldPos, sf::Vector2f oldSize) override {
        m_Lines.setSize(m_Size - (m_Theme.offset + m_Theme.pad));
    }

    TextBox m_Lines;
};

int main() {
    Theme::AllThemes& themes = Theme::Get<Theme::AllThemes>();
    uint32_t& windowWidth = themes.windowWidth;
    uint32_t& windowHeight = themes.windowHeight;

    auto window = sf::RenderWindow(sf::VideoMode({ windowWidth, windowHeight }), "Visionary");
    window.setVerticalSyncEnabled(true);

    TextEditor editor({0, 0}, {static_cast<float>(windowWidth), static_cast<float>(windowHeight)});
    sf::Clock deltaClock, clock; 

    const auto onClose = [&window](const sf::Event::Closed& closedEvent) {
        window.close();
    };

    const auto onResize = [&window, &windowWidth, &windowHeight, &editor](const sf::Event::Resized& resizedEvent) {
        windowWidth = resizedEvent.size.x; windowHeight = resizedEvent.size.y;
		auto size = sf::Vector2f(static_cast<float>(windowWidth),
                                 static_cast<float>(windowHeight));
        editor.setSize(size);
	};

    const auto onMouseWheelScroll = [&window, &editor](const sf::Event::MouseWheelScrolled& mouseWheelEvent) {
        editor.onMouseWheelScroll(mouseWheelEvent);
    };

    const auto onKeyPressed = [&editor](const sf::Event::KeyPressed& keyPressedEvent) {
		editor.onKeyPressed(keyPressedEvent);
    };

    const auto onTextEntered = [&editor](const sf::Event::TextEntered& textEnteredEvent) {
        editor.onTextEntered(textEnteredEvent);
    };

    while (window.isOpen()) {
        double deltaTime = deltaClock.restart().asSeconds();
        window.handleEvents(onClose, onResize, onMouseWheelScroll, onKeyPressed, onTextEntered);

        editor.update(deltaTime);
        
        window.clear(sf::Color(0, 0, 0));
        
        window.draw(editor);

        window.display();
    }

    return 0;
}
