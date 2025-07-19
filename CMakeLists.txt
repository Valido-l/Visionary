cmake_minimum_required(VERSION 3.28)
project(CMakeSFMLProject LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include(FetchContent)
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 3.0.1
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM)
FetchContent_MakeAvailable(SFML)

FetchContent_Declare(
    nlohmann_json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG        v3.11.3)         
FetchContent_MakeAvailable(nlohmann_json)

add_executable(main "src/main.cpp" "src/TextBox.h" "src/TextBox.cpp" "src/Drawable.hpp" "src/Cursor.h" "src/Text.h" "src/Text.cpp"  "src/Cursor.cpp" "src/CursorLocation.hpp" "src/LineIndicator.h" "src/LineIndicator.cpp")
target_compile_features(main PRIVATE cxx_std_17)
target_link_libraries(main PRIVATE SFML::Graphics nlohmann_json::nlohmann_json)

add_custom_command(TARGET main POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/Fonts
            $<TARGET_FILE_DIR:main>/Fonts)
			
add_custom_command(TARGET main POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/Themes
            $<TARGET_FILE_DIR:main>/Themes)
