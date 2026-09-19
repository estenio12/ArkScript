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
#include <vector>
#include <cstdint>
#include <memory>
#include <fstream>
#include <sstream>
#include <filesystem>
#include "FileHandler.hpp"
#include "Output.hpp"
#include "Global.hpp"
#include "Tools.hpp"

namespace ArkScript
{
    enum class TokenType : uint8_t
    {
        LITERAL_INT,
        LITERAL_FLOAT,
        LITERAL_BOOL,
        LITERAL_CHAR,
        LITERAL_STRING,
        KEYWORD,
        OP_ARITHMETIC,
        OP_LOGICAL,
        OP_ASSIGNMENT,
        OP_COMPARISON,
        OP_BITWISE,
        DELIMITER,
        IDENTIFIER,
        END_OF_FILE, 
        UNKNOWN
    };

    struct Token
    {
        std::string content;
        TokenType type;
        uint32_t line;
        uint32_t col;
    };

    class TokenManager
    {
        private:
            std::string source_file_path;
            std::vector<Token> tokens;
            size_t cursor = 0;

        public:
            TokenManager(const std::string& source_file_path, size_t reserve)
            {
                this->source_file_path = source_file_path;
                // # Pre-reserve memory space to token list.
                this->tokens.reserve(reserve);
            }

        public:
            void PushToken(Token token){ this->tokens.push_back(token); }
            bool IsAtEnd() const { return cursor >= tokens.size(); }
        
            const Token& Peek(size_t offset = 0) const 
            {
                if (cursor + offset >= tokens.size()) return tokens.back();
                return tokens[cursor + offset];
            }
        
            const Token& Consume() 
            {
                if (cursor < tokens.size()) return tokens[cursor++];
                return tokens.back();
            }
        
            void Advance() 
            {
                if (cursor < tokens.size()) cursor++;
            }

            void PushEndOfFile(int64_t line = 0, int64_t col = 0)
            {
                ArkScript::Token token;
                token.col = col;
                token.line = line;
                token.content = "\0";
                token.type = TokenType::END_OF_FILE;

                this->tokens.push_back(token);
            }

            void PrintTokens()
            {
                if(ArkScript::Global::PRINT_LEXER_OUTPUT == ArkScript::Global::OUTPUT_FLAG::NONE) return;

                std::stringstream ss;
                for(const auto& token : this->tokens)
                {
                    ss << "\nContent: " << token.content
                       << "\nType: " << TokenTypeString(token.type)
                       << "\nLine: " << token.line
                       << "\nCol: "  << token.col
                       << "\n------------------------\n\n";
                }

                if(ArkScript::Global::PRINT_LEXER_OUTPUT == ArkScript::Global::OUTPUT_FLAG::FILE)
                {
                    std::filesystem::path outputPath = ArkScript::Global::PRINT_LEXER_OUTPUT_FILE;
                    std::filesystem::create_directories(outputPath.parent_path());
                    uint64_t hashValue = ArkScript::Tools::Hash::GenerateHash(ss.str());
                    std::ofstream file(ArkScript::Global::PRINT_LEXER_OUTPUT_FILE, std::ios::binary);
                    if(file.is_open())
                    {
                        file << std::hex << hashValue << "\n";
                        file << ss.rdbuf();
                    }
                    file.close();
                }
                else
                    ArkScript::Output::Print(ss.str());
            }

            std::string GetFilePath() { return this->source_file_path; }

        private:
            std::string TokenTypeString(TokenType type)
            {
                if(type == TokenType::LITERAL_INT) return "LITERAL_INT";
                if(type == TokenType::LITERAL_FLOAT) return "LITERAL_FLOAT";
                if(type == TokenType::LITERAL_BOOL) return "LITERAL_BOOL";
                if(type == TokenType::LITERAL_CHAR) return "LITERAL_CHAR";
                if(type == TokenType::LITERAL_STRING) return "LITERAL_STRING";
                if(type == TokenType::KEYWORD) return "KEYWORD";
                if(type == TokenType::OP_ARITHMETIC) return "OP_ARITHMETIC";
                if(type == TokenType::OP_LOGICAL) return "OP_LOGICAL";
                if(type == TokenType::OP_ASSIGNMENT) return "OP_ASSIGNMENT";
                if(type == TokenType::OP_COMPARISON) return "OP_COMPARISON";
                if(type == TokenType::OP_BITWISE) return "OP_BITWISE";
                if(type == TokenType::DELIMITER) return "DELIMITER";
                if(type == TokenType::IDENTIFIER) return "IDENTIFIER";
                if(type == TokenType::END_OF_FILE) return "END_OF_FILE";
                return "UNKNOWN";
            }
    };
}