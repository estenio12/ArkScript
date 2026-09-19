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
#include <cstdint>
#include <memory>
#include <unordered_set>
#include "FileHandler.hpp"
#include "TokenManager.hpp"

namespace ArkScript
{
    class Lexer
    {
        private:
            std::string_view source;
            std::shared_ptr<ArkScript::FileDescriptor> file_descriptor;
            std::unique_ptr<ArkScript::TokenManager> tokens;

        public:
            Lexer(const std::string& source_file_path)
            {
                file_descriptor = FileHandler::GetFileContent(source_file_path);
                source = file_descriptor->source_code;
                tokens = std::make_unique<ArkScript::TokenManager>(file_descriptor->absolute_path, file_descriptor->file_size / 5);
            }
            std::unique_ptr<ArkScript::TokenManager> Tokenize();

        private:
            const char COMMENTARY = '#';
            const char WHITESPACE = ' ';
            const char TAB = '\t';
            const char RETURN = '\r';
            const char NEW_LINE = '\n';
            const char FORM_FEED = '\f';
            const char VERTICAL_TAB = '\v';

        private:
            bool IsNumber(const std::string& target, uint8_t max_dots);
            bool IsInteger(const std::string& target);
            bool IsFloat(const std::string& target);
            bool IsBoolean(const std::string& target);
            bool IsChar(const std::string& target);
            bool IsString(const std::string& target);
            bool IsKeyword(const std::string& target);
            bool IsOpArithmetic(const size_t& index);
            uint8_t IsDelimiter(const size_t& index);
            uint8_t IsOpLogic(const size_t& index);
            uint8_t IsOpComparison(const size_t& index);
            uint8_t IsOpBitwise(const size_t& index);
            uint8_t IsOpAssignment(const size_t& index);
            bool IsIdentifier(const std::string& target);

        private:
            ArkScript::TokenType FindType(const std::string& target);
            std::string GetLexeme(std::string& buffer);
            void BuildToken(std::string lexeme, uint64_t line, uint64_t col, ArkScript::TokenType type = ArkScript::TokenType::UNKNOWN);
            size_t GetUTF8Length(const std::string& str);
    };
}


