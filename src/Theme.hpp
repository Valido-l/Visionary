#pragma once

#include <SFML/Graphics.hpp>

#include "Config.hpp"

namespace nlohmann {
    template <>
    struct adl_serializer<sf::Color> {
        // Serialise as [r, g, b, a].
        static void to_json(json& j, const sf::Color& c) {
            j = json::array({ c.r, c.g, c.b, c.a });
        }

        // Parse from [r, g, b, a].
        static void from_json(const json& j, sf::Color& c) {
            if (!j.is_array() || j.size() != 4)
                c = sf::Color(0, 0, 0, 0);

            c.r = j.at(0).get<uint8_t>();
            c.g = j.at(1).get<uint8_t>();
            c.b = j.at(2).get<uint8_t>();
            c.a = j.at(3).get<uint8_t>();
        }
    };

    template <>
    struct adl_serializer<sf::Vector2f> {
        // Serialize as {"x":..., "y":...}.
        static void to_json(json& j, const sf::Vector2f& v) {
            j = { {"x", v.x}, {"y", v.y} };
        }

        // Parse from {"x":..., "y":...}.
        static void from_json(const json& j, sf::Vector2f& v) {
            j.at("x").get_to(v.x);
            j.at("y").get_to(v.y);
        }
    };
};

namespace Theme {
    struct CursorTheme {
        float cursorWidth = 2.0f;
        float outlineThickness = 0.0f;

        sf::Color cursorColor = { 200, 200, 200 };
        sf::Color outlineColor = { 100, 0, 0 };
    };

    struct LineIndicatorTheme {
        float padLeft = 25.0f;
        float padRight = 10.0f;
        float outlineThickness = 1.0f;

        sf::Color textColor = { 135, 135, 135 };
        sf::Color backgroundColor = { 10, 10, 10 };
        sf::Color outlineColor = { 200, 5, 40 };
    };

    struct TextBoxTheme {
        uint32_t fontSize = 24;
        float lineIndicatorPad = 20.0f;
        float lineMargin = 5.0f;

        sf::Color textColor = { 180, 180, 180 };
        sf::Color backgroundColor = { 25, 25, 25 };
        sf::Color lineHighlightColor = { 70, 70, 70, 70 };
        sf::Color selectedTextColor = { 80, 165, 245, 70 };
    };

    struct TextEditorTheme {
        sf::Vector2f offset = { 0.0f, 0.0f };
        sf::Vector2f pad = { 0.0f, 0.0f };
    };

    struct AllThemes {
        std::string fontName = "CascadiaCode.ttf";
        uint32_t windowWidth = 800;
        uint32_t windowHeight = 600;
        float scale = 1.0f;

        CursorTheme cursor;
        LineIndicatorTheme lineIndicator;
        TextBoxTheme textBox;
        TextEditorTheme textEditor;
    };

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(CursorTheme, cursorWidth, outlineThickness,
        cursorColor, outlineColor)

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(LineIndicatorTheme,
        padLeft, padRight, outlineThickness,
        textColor, backgroundColor, outlineColor)

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(TextBoxTheme,
        fontSize, lineIndicatorPad, lineMargin,
        textColor, backgroundColor, lineHighlightColor, selectedTextColor)

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(TextEditorTheme, offset, pad)

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(AllThemes,
            fontName, windowWidth, windowHeight, scale,
            cursor, lineIndicator, textBox)

    template <typename T> 
    inline T& Get() noexcept;

    template <>
    inline AllThemes& Get<AllThemes>() noexcept {
        static AllThemes themes;
        static bool firstCall = true;

        if (firstCall) {
            firstCall = false;
            const auto& path = "Themes/" + Config::Get().themeName;
            try {
                if (std::filesystem::exists(path)) {
                    std::ifstream in(path);
                    if (!in) throw std::runtime_error("Cannot open theme '" + path + "', using defaults.");

                    nlohmann::json j;
                    in >> j;
                    themes = j.get<AllThemes>();
                }
                else {
                    nlohmann::json j = themes;
                    std::ofstream out(path);
                    if (!out) throw std::runtime_error("Cannot create theme '" + path + "', using defaults");
                    out << j.dump(4);
                }
            }
            catch (const std::exception& e) {
                std::cerr << "[THEME]: " << e.what() << std::endl;
            }
        }

        return themes;
    }

    template<> 
    inline CursorTheme& Get<CursorTheme>() noexcept {
        return Get<AllThemes>().cursor;
    }

    template<>
    inline LineIndicatorTheme& Get<LineIndicatorTheme>() noexcept {
        return Get<AllThemes>().lineIndicator;
    }

    template<>
    inline TextBoxTheme& Get<TextBoxTheme>() noexcept {
        return Get<AllThemes>().textBox;
    }

    template<>
    inline TextEditorTheme& Get<TextEditorTheme>() noexcept {
        return Get<AllThemes>().textEditor;
    }
};

template <typename T>
class Stylable {
public:
    Stylable() : m_Theme(Theme::Get<T>()) {}

    virtual ~Stylable() = default;

    const T& getTheme() const noexcept {
        return m_Theme;
    }

    T& getTheme() noexcept {
        return m_Theme;
    }

protected:
    T m_Theme;
};
