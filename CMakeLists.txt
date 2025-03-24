cmake_minimum_required(VERSION 3.10)
project(2048 CXX)

set(CMAKE_CXX_STANDARD 17) # Use C++17
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(cmake/UpdateSubmodules.cmake)

# Set CMAKE_BUILD_TYPE to Debug if not specified
if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Choose the type of build." FORCE)
    message(STATUS "CMAKE_BUILD_TYPE not specified, defaulting to Debug")
endif()

set(RUNTIME_OUTPUT_DIR "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${RUNTIME_OUTPUT_DIR})
set(DESTINATION_PATH ${RUNTIME_OUTPUT_DIR}/$<CONFIG>)

add_subdirectory(extern/SDL3)
add_subdirectory(extern/SDL3_ttf)

# Specify source files
set(SOURCES
    Game.cpp
)

# Create the executable
add_executable(2048 ${SOURCES})

target_include_directories(2048 PRIVATE ${CMAKE_SOURCE_DIR}/extern/SDL3/include/)
target_include_directories(2048 PRIVATE ${CMAKE_SOURCE_DIR}/extern/SDL3_ttf/include/)

# Link SDL3 to your project
target_link_libraries(2048 SDL3::SDL3 SDL3_ttf::SDL3_ttf)

set_target_properties(2048 PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${RUNTIME_OUTPUT_DIR}
)

# Platform-specific settings
if (WIN32)
#     # Set SDL paths manually (Windows only)
     set(FONT_PATH "${CMAKE_SOURCE_DIR}/ComicSansMS.ttf")

#     # Copy SDL3 and SDL3_ttf DLLs on Windows
     add_custom_command(TARGET 2048 POST_BUILD
         COMMAND ${CMAKE_COMMAND} -E copy_if_different ${FONT_PATH} ${DESTINATION_PATH}
    )

endif()
