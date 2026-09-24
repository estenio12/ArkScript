cmake_minimum_required(VERSION 3.15)
project(ArkScript LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_USE_RESPONSE_FILE_FOR_INCLUDES OFF)

set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
# set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)

if(NOT MSVC)
    add_compile_options(-Wall -Wextra -Wpedantic)
else()
    add_compile_options(/W4)
endif()

include_directories(arkc/src)

add_executable(arkc
    arkc/src/Compiler.cpp
    arkc/src/Lexer.cpp
    arkc/src/Parser.cpp
    arkc/src/ParserHelpers.cpp
    arkc/src/FileHandler.cpp
    arkc/src/main.cpp
    arkc/src/WindowsHandler.cpp)

set(BIN_DIR "${CMAKE_CURRENT_SOURCE_DIR}/arkc/bin")
set_target_properties(arkc PROPERTIES 
    RUNTIME_OUTPUT_DIRECTORY "${BIN_DIR}"
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${BIN_DIR}"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${BIN_DIR}"
    OUTPUT_NAME arkc
)
