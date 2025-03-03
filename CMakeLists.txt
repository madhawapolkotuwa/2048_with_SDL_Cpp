cmake_minimum_required(VERSION 3.10)
project(2048 CXX)

set(CMAKE_CXX_STANDARD 17) # Use C++17
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(DESTINATION_PATH "${CMAKE_BINARY_DIR}/Debug")

# Add SDL3 cmake path to CMAKE_PREFIX_PATH
set(SDL3_DIR "C:/SDL3/SDL3.2.4/cmake") # SDL3 dir
set(SDL3_ttf_DIR "C:/SDL3/SDL3_ttf3.1.0/cmake") # SDL3 ttf dir

set(SDL3_DLL_PATH "C:/SDL3/SDL3.2.4/lib/x64/SDL3.dll") #  SDL3 dll
set(SDL3_ttf_DLL_PATH "C:/SDL3/SDL3_ttf3.1.0/lib/x64/SDL3_ttf.dll") # SDL3_ttf DLL
set(FONT_PATH "${CMAKE_SOURCE_DIR}/ComicSansMS.ttf") # Font

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

# Copy SDL3_ttf.dll
add_custom_command(
    TARGET 2048 POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    ${SDL3_DLL_PATH} ${DESTINATION_PATH}
)

# Copy SDL3.dll
add_custom_command(
    TARGET 2048 POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    ${SDL3_ttf_DLL_PATH} ${DESTINATION_PATH}
)

# Copy ComicSansMS.ttf
add_custom_command(
    TARGET 2048 POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    ${FONT_PATH} ${DESTINATION_PATH}
)