cmake_minimum_required(VERSION 3.15)
project(ArkLang LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_USE_RESPONSE_FILE_FOR_INCLUDES OFF)

if(NOT MSVC)
    add_compile_options(-Wall -Wextra -Wpedantic)
else()
    add_compile_options(/W4)
endif()

include_directories(arkc/src)

set(SOURCES
    src/CoreCompile.cpp
    src/FileHandler.cpp
    src/Lexer.cpp
    src/main.cpp
    src/ModuleManager.cpp
    src/Parser.cpp
    src/ParserHelpers.cpp
)

add_executable(arkc ${SOURCES})

set(BIN_DIR "${CMAKE_CURRENT_SOURCE_DIR}/arkc/bin")
set_target_properties(arkc PROPERTIES 
    RUNTIME_OUTPUT_DIRECTORY "${BIN_DIR}"
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${BIN_DIR}"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${BIN_DIR}"
    OUTPUT_NAME arkc
)
