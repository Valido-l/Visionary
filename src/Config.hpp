#pragma once

#include <nlohmann/json.hpp>
#include <filesystem>
#include <fstream>
#include <cstdint>
#include <iostream>
#include <string>

namespace Config {
    struct Properties {
        std::string themeName = "default.json";
        std::string defaultText = "Hello, World!";
        uint32_t tabWidth = 4;
    };

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Properties, themeName, defaultText, tabWidth)

    inline Properties& Get() {
        static Properties properties; 
        static bool firstCall = true;

        if (firstCall) {
            firstCall = false;
            constexpr const char* path = "config.json";
            try {
                if (std::filesystem::exists(path)) {
                    std::ifstream in(path);
                    if (!in) throw std::runtime_error("Cannot open config.json, using defaults.");

                    nlohmann::json j;
                    in >> j;                   
                    properties = j.get<Properties>();
                }
                else {
                    nlohmann::json j = properties;
                    std::ofstream out(path);
                    if (!out) throw std::runtime_error("Cannot create config.json, using defaults.");
                    out << j.dump(4);
                }
            }
            catch (const std::exception& e) {
                std::cerr << "[CONFIG]: " << e.what() << std::endl;
            }
        }

        return properties;
    }
};

