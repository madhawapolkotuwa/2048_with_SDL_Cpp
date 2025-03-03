cmake_minimum_required(VERSION 3.10)
project(2048 CXX)

set(CMAKE_CXX_STANDARD 17) # Use C++17
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Set default destination path based on platform
if (WIN32)
    set(DESTINATION_PATH "${CMAKE_BINARY_DIR}/Debug")
    set(SDL3_DIR "C:/SDL3/SDL3.2.4/cmake") # SDL3 CMake config path
    set(SDL3_ttf_DIR "C:/SDL3/SDL3_ttf3.1.0/cmake") # SDL3_ttf CMake config path
    list(APPEND CMAKE_PREFIX_PATH ${SDL3_DIR} ${SDL3_ttf_DIR})
elseif (APPLE)
    set(DESTINATION_PATH "${CMAKE_BINARY_DIR}/2048.app/Contents/MacOS")
else()
    set(DESTINATION_PATH "${CMAKE_BINARY_DIR}/bin")
endif()

find_package(SDL3 REQUIRED)
find_package(SDL3_ttf REQUIRED)

# Specify source files
set(SOURCES
    Game.cpp
)

# Create the executable
add_executable(2048 ${SOURCES})

# Link SDL3 to your project
target_link_libraries(2048 SDL3::SDL3 SDL3_ttf::SDL3_ttf)

# Platform-specific settings
if (WIN32)
    # Set SDL paths manually (Windows only)
    set(SDL3_DLL_PATH "C:/SDL3/SDL3.2.4/lib/x64/SDL3.dll")
    set(SDL3_ttf_DLL_PATH "C:/SDL3/SDL3_ttf3.1.0/lib/x64/SDL3_ttf.dll")
    set(FONT_PATH "${CMAKE_SOURCE_DIR}/ComicSansMS.ttf")

    # Copy SDL3 and SDL3_ttf DLLs on Windows
    add_custom_command(TARGET 2048 POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${SDL3_DLL_PATH} ${DESTINATION_PATH}
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${SDL3_ttf_DLL_PATH} ${DESTINATION_PATH}
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${FONT_PATH} ${DESTINATION_PATH}
    )

elseif (APPLE)
    # macOS-specific settings
    set_target_properties(2048 PROPERTIES
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_GUI_IDENTIFIER "com.game.2048"
        MACOSX_BUNDLE_BUNDLE_NAME "2048"
        MACOSX_BUNDLE_BUNDLE_VERSION "1.0"
        MACOSX_BUNDLE_SHORT_VERSION_STRING "1.0"
    )

    # Ensure SDL3 dylibs are copied into the bundle
    add_custom_command(TARGET 2048 POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different /opt/homebrew/lib/libSDL3.dylib ${DESTINATION_PATH}
        COMMAND ${CMAKE_COMMAND} -E copy_if_different /opt/homebrew/lib/libSDL3_ttf.dylib ${DESTINATION_PATH}
    )

elseif (UNIX AND NOT APPLE)
    # Linux-specific settings
    add_custom_command(TARGET 2048 POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different /usr/lib/libSDL3.so ${DESTINATION_PATH}
        COMMAND ${CMAKE_COMMAND} -E copy_if_different /usr/lib/libSDL3_ttf.so ${DESTINATION_PATH}
    )
endif()