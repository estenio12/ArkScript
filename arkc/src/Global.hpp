/*
 * Copyright 2026 ArkScript Authors
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 *     http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#pragma once
#include <string>
#include <cstdint>

namespace ArkScript::Global
{
    enum class OUTPUT_FLAG : uint8_t { STDOUT, FILE, NONE };

    inline std::string BUILD_ID = "ArkScript Compiler (C)";
    inline std::string BUILD_VERSION = "0.1.0";
    inline std::string BUILD_DATE = __DATE__;
    inline std::string BUILD_LICENSE = "Apache 2.0 - Copyright: ArkScript Authors";

    inline bool PRINT_TIMER = true;

    inline std::string SOURCE_FILE = "";
    inline std::string OUTPUT_COMPILATION_FILE = "";
    
    inline OUTPUT_FLAG PRINT_LEXER_OUTPUT = OUTPUT_FLAG::STDOUT;
    inline std::string PRINT_LEXER_OUTPUT_FILE = "";
    
    inline OUTPUT_FLAG PRINT_PARSER_OUTPUT = OUTPUT_FLAG::NONE;
    inline std::string PRINT_PARSER_OUTPUT_FILE = "";               
}