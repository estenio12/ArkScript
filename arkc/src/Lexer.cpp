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
#include <string>
#include "Lexer.hpp"
#include "Definitions.hpp"
#include "Output.hpp"

std::unique_ptr<ArkScript::TokenManager> ArkScript::Lexer::Tokenize()
{
    uint64_t line = 1;
    uint64_t col  = 0;
    std::string buffer;
    buffer.reserve(256);

    size_t source_size = file_descriptor->source_code.size();
    for(size_t i = 0; i < source_size; i++)
    {
        char letter = source[i];
        
        if((static_cast<unsigned char>(letter) & 0xC0) != 0x80) col++;
        
        if(letter == TAB || letter == RETURN) continue;

        if(letter == NEW_LINE || letter == FORM_FEED || letter == VERTICAL_TAB)
        {
            this->BuildToken(this->GetLexeme(buffer), line, col - 1);
            line++;
            col = 0;
            continue;
        }

        if(letter == COMMENTARY)
        {
            while (i < source_size && source[i] != NEW_LINE) 
            { 
                i++; 
                col = 0;
            }
            line++;
            continue;
        }

        if(letter == WHITESPACE)
        {
            this->BuildToken(this->GetLexeme(buffer), line, col);
            continue;
        }

        // # Char identifier.
        if(letter == ArkScript::DELIMITER::QUOTE[0])
        {
            this->BuildToken(this->GetLexeme(buffer), line, col - 1);

            std::string slice;
            bool is_escaped = false;
            while((i + 1) < source_size)
            {
                i++; col++;
                char current = source[i];
                
                if (current == ArkScript::DELIMITER::QUOTE[0] && !is_escaped) break;
                
                slice.push_back(current);

                if (current == '\\')
                    is_escaped = !is_escaped;
                else
                    is_escaped = false;
            }
            auto lexime = this->GetLexeme(slice);
            auto type = lexime.size() > 0 ? ArkScript::TokenType::LITERAL_CHAR : ArkScript::TokenType::UNKNOWN;
            this->BuildToken(lexime, line, col, type);
            continue;
        }

        // # String identifier.
        if(letter == ArkScript::DELIMITER::DOUBLEQUOTE[0])
        {
            this->BuildToken(this->GetLexeme(buffer), line, col - 1);

            std::string slice;
            bool is_escaped = false;
            while((i + 1) < source_size)
            {
                i++;
                char current = source[i];

                if ((static_cast<unsigned char>(current) & 0xC0) != 0x80) col++;
                
                if (current == ArkScript::DELIMITER::DOUBLEQUOTE[0] && !is_escaped) break;
                
                slice.push_back(current);

                if (current == '\\')
                    is_escaped = !is_escaped;
                else
                    is_escaped = false;
            }
            this->BuildToken(this->GetLexeme(slice), line, col, ArkScript::TokenType::LITERAL_STRING);
            continue;
        }

        auto delimiter = IsDelimiter(i);
        if(delimiter > 0)
        {
            this->BuildToken(this->GetLexeme(buffer), line, col);
            
            if(delimiter == 1)
            {
                this->BuildToken(std::string {letter}, line, col + 1, ArkScript::TokenType::DELIMITER);
            }
            else
            {
                this->BuildToken(std::string { letter, this->source[i + 1] }, line, col + 1, ArkScript::TokenType::DELIMITER);
                i++;
            }

            continue;
        }

        // # Build negative numbers.
        if(letter == ArkScript::OP_ARITHMETIC::SUB[0] && (i + 1) < source_size && ArkScript::Tools::Number::IsDigit(source[i + 1]))
        {
            buffer.push_back(letter);
            continue;
        }

        if(IsOpArithmetic(i))
        {
            this->BuildToken(this->GetLexeme(buffer), line, col - 1);
            this->BuildToken(std::string{letter}, line, col, ArkScript::TokenType::OP_ARITHMETIC);
            continue;
        }

        auto logical = IsOpLogic(i);
        if(logical > 0)
        {
            this->BuildToken(this->GetLexeme(buffer), line, col - 1);
            
            if(logical == 1)
            {
                this->BuildToken(std::string {letter}, line, col + 1, ArkScript::TokenType::OP_LOGICAL);
            }
            else
            {
                this->BuildToken(std::string { letter, this->source[i + 1] }, line, col + 1, ArkScript::TokenType::OP_LOGICAL);
                i++;
            }

            continue;
        }

        auto assignment = IsOpAssignment(i);
        if(assignment > 0)
        {
            this->BuildToken(this->GetLexeme(buffer), line, col - 1);
            
            if(assignment == 1)
            {
                this->BuildToken(std::string {letter}, line, col + 1, ArkScript::TokenType::OP_ASSIGNMENT);
            }
            else
            {
                this->BuildToken(std::string { letter, this->source[i + 1] }, line, col + 1, ArkScript::TokenType::OP_ASSIGNMENT);
                i++;
            }

            continue;
        }

        auto comparison = IsOpComparison(i);
        if(comparison > 0)
        {
            this->BuildToken(this->GetLexeme(buffer), line, col - 1);
            
            if(comparison == 1)
            {
                this->BuildToken(std::string {letter}, line, col + 1, ArkScript::TokenType::OP_COMPARISON);
            }
            else
            {
                this->BuildToken(std::string { letter, this->source[i + 1] }, line, col + 1, ArkScript::TokenType::OP_COMPARISON);
                i++;
            }

            continue;
        }

        auto bitwise = IsOpBitwise(i);
        if(bitwise > 0)
        {
            this->BuildToken(this->GetLexeme(buffer), line, col - 1);
            
            if(bitwise == 1)
            {
                this->BuildToken(std::string {letter}, line, col + 1, ArkScript::TokenType::OP_BITWISE);
            }
            else
            {
                this->BuildToken(std::string { letter, this->source[i + 1] }, line, col + 1, ArkScript::TokenType::OP_BITWISE);
                i++;
            }

            continue;
        }

        buffer.push_back(letter);
    }

    if(buffer.size() > 0)
        this->BuildToken(this->GetLexeme(buffer), line, col);

    this->tokens->PushEndOfFile(line, col);
    this->tokens->PrintTokens();
    return std::move(this->tokens);
}

void ArkScript::Lexer::BuildToken(std::string lexeme, uint64_t line, uint64_t col, ArkScript::TokenType type)
{
    if(lexeme.empty() && type != ArkScript::TokenType::LITERAL_STRING) return;

    ArkScript::Token token;
    token.col = col - this->GetUTF8Length(lexeme);
    token.line = line;
    token.content = lexeme;

    if(type == ArkScript::TokenType::UNKNOWN)
        token.type = this->FindType(token.content);
    else
        token.type = type;

    this->tokens->PushToken(token);
}

size_t ArkScript::Lexer::GetUTF8Length(const std::string& str) 
{
    size_t len = 0;
    for (unsigned char c : str) { if ((c & 0xC0) != 0x80) len++; }
    return len;
}

std::string ArkScript::Lexer::GetLexeme(std::string& buffer)
{
    auto lexeme = buffer;
    buffer.clear();
    return lexeme;
}

ArkScript::TokenType ArkScript::Lexer::FindType(const std::string& target)
{
    if(ArkScript::Lexer::IsInteger(target)) return ArkScript::TokenType::LITERAL_INT;
    if(ArkScript::Lexer::IsFloat(target)) return ArkScript::TokenType::LITERAL_FLOAT;
    if(ArkScript::Lexer::IsBoolean(target)) return ArkScript::TokenType::LITERAL_BOOL;
    if(ArkScript::Lexer::IsKeyword(target)) return ArkScript::TokenType::KEYWORD;
    if(ArkScript::Lexer::IsIdentifier(target)) return ArkScript::TokenType::IDENTIFIER;

    return ArkScript::TokenType::UNKNOWN;
}

bool ArkScript::Lexer::IsNumber(const std::string& target, uint8_t max_dots)
{
    if (target.empty()) return false;

    uint8_t dots_found = 0;

    for(size_t i = 0; i < target.size(); i++)
    {
        char letter = target[i];
        
        if(letter == ArkScript::OP_ARITHMETIC::SUB[0] && i == 0 && (i + 1) < target.size() && 
           ArkScript::Tools::Number::IsDigit(target[i + 1]))
        {
            continue;
        }

        if(!ArkScript::Tools::Number::IsDigit(letter))
        {
            if(letter == '.')
            {
                dots_found++;
                if(dots_found > max_dots) return false;
            }
            else
            {
                return false;
            }
        }
    }

    return max_dots == 0 ? true : (dots_found == max_dots);
}

bool ArkScript::Lexer::IsInteger(const std::string& target)
{
    return ArkScript::Lexer::IsNumber(target, 0);
}

bool ArkScript::Lexer::IsFloat(const std::string& target)
{
    if(!ArkScript::Lexer::IsNumber(target, 1)) return false;
    if(target.front() == '.' || target.back() == '.') return false;
    return true;
}

bool ArkScript::Lexer::IsBoolean(const std::string& target)
{
    return target == ArkScript::LITERAL_VALUES::TRUE || target == ArkScript::LITERAL_VALUES::FALSE;
}

bool ArkScript::Lexer::IsKeyword(const std::string& target)
{
    return ArkScript::KEYWORDS_LIST.find(target) != ArkScript::KEYWORDS_LIST.end();
}

bool ArkScript::Lexer::IsOpArithmetic(const size_t& index)
{
    char letter = source[index];
    char next = (index + 1 < this->source.size()) ? source[index + 1] : '\0';

    if(next != '\0')
    {
        if((letter == ArkScript::OP_ARITHMETIC::ADD[0] && next == ArkScript::OP_ASSIGNMENT::ASSIGN[0]) || 
           (letter == ArkScript::OP_ARITHMETIC::SUB[0] && next == ArkScript::OP_ASSIGNMENT::ASSIGN[0]) ||
           (letter == ArkScript::OP_ARITHMETIC::MUL[0] && next == ArkScript::OP_ASSIGNMENT::ASSIGN[0]) ||
           (letter == ArkScript::OP_ARITHMETIC::DIV[0] && next == ArkScript::OP_ASSIGNMENT::ASSIGN[0]) )
        {
            return false;
        }
    }
    
    if(letter == ArkScript::OP_ARITHMETIC::ADD[0] ||
       letter == ArkScript::OP_ARITHMETIC::SUB[0] ||
       letter == ArkScript::OP_ARITHMETIC::MUL[0] ||
       letter == ArkScript::OP_ARITHMETIC::DIV[0] )
    {
        return true;
    }

    return false;
}

uint8_t ArkScript::Lexer::IsOpLogic(const size_t& index)
{
    char letter = source[index];
    char next = (index + 1 < this->source.size()) ? source[index + 1] : '\0';

    if(next != '\0')
    {
        if(ArkScript::OP_LOGICAL::OR[0] == letter && ArkScript::OP_LOGICAL::OR[1] == next) return 2;
        if(ArkScript::OP_LOGICAL::AND[0] == letter && ArkScript::OP_LOGICAL::AND[1] == next) return 2;
    }

    if(letter == ArkScript::OP_LOGICAL::NOT[0] && next != ArkScript::OP_ASSIGNMENT::ASSIGN[0]) return 1;
    return 0;
}

uint8_t ArkScript::Lexer::IsOpComparison(const size_t& index)
{
    char letter = source[index];
    char next = (index + 1 < this->source.size()) ? source[index + 1] : '\0';

    if(next != '\0')
    {
        if(ArkScript::OP_COMPARISON::EQ[0]   == letter && ArkScript::OP_COMPARISON::EQ[1]   == next) return 2;
        if(ArkScript::OP_COMPARISON::DIFF[0] == letter && ArkScript::OP_COMPARISON::DIFF[1] == next) return 2;
        if(ArkScript::OP_COMPARISON::GTEQ[0] == letter && ArkScript::OP_COMPARISON::GTEQ[1] == next) return 2;
        if(ArkScript::OP_COMPARISON::LTEQ[0] == letter && ArkScript::OP_COMPARISON::LTEQ[1] == next) return 2;
    }

    if(letter == ArkScript::OP_COMPARISON::GT[0]) return 1;
    if(letter == ArkScript::OP_COMPARISON::LT[0]) return 1;
    return 0;
}

uint8_t ArkScript::Lexer::IsOpBitwise(const size_t& index)
{
    char letter = source[index];
    char next = (index + 1 < this->source.size()) ? source[index + 1] : '\0';

    if(next != '\0')
    {
        if(ArkScript::OP_BITWISE::LSHIFT[0] == letter && ArkScript::OP_BITWISE::LSHIFT[1] == next) return 2;
        if(ArkScript::OP_BITWISE::RSHIFT[0] == letter && ArkScript::OP_BITWISE::RSHIFT[1] == next) return 2;
    }

    if(letter == ArkScript::OP_BITWISE::OR[0])  return 1;
    if(letter == ArkScript::OP_BITWISE::AND[0]) return 1;
    if(letter == ArkScript::OP_BITWISE::XOR[0]) return 1;
    if(letter == ArkScript::OP_BITWISE::NOT[0]) return 1;
    return 0;
}

uint8_t ArkScript::Lexer::IsDelimiter(const size_t& index)
{
    char letter = source[index];
    char next = (index + 1 < this->source.size()) ? source[index + 1] : '\0';

    if(next != '\0')
    {
        if(ArkScript::DELIMITER::ARROW[0] == letter && ArkScript::DELIMITER::ARROW[1] == next) return 2;
        if(ArkScript::DELIMITER::SCOPEACCESS[0] == letter && ArkScript::DELIMITER::SCOPEACCESS[1] == next) return 2;
    }

    if(letter == ArkScript::DELIMITER::COMMA[0]) return 1;
    if(letter == ArkScript::DELIMITER::COLON[0]) return 1;
    if(letter == ArkScript::DELIMITER::LPARAN[0]) return 1;
    if(letter == ArkScript::DELIMITER::RPARAN[0]) return 1;
    if(letter == ArkScript::DELIMITER::LBRACE[0]) return 1;
    if(letter == ArkScript::DELIMITER::RBRACE[0]) return 1;
    if(letter == ArkScript::DELIMITER::LBRACKET[0]) return 1;
    if(letter == ArkScript::DELIMITER::RBRACKET[0]) return 1;
    if(letter == ArkScript::DELIMITER::SEMICOLON[0]) return 1;
    if(letter == ArkScript::DELIMITER::QUOTE[0]) return 1;
    if(letter == ArkScript::DELIMITER::DOUBLEQUOTE[0]) return 1;

    return 0;
}

uint8_t ArkScript::Lexer::IsOpAssignment(const size_t& index)
{
    char letter = source[index];
    char next = (index + 1 < this->source.size()) ? source[index + 1] : '\0';

    if(next != '\0')
    {
        if(ArkScript::OP_ASSIGNMENT::ADDASSIGN[0] == letter && ArkScript::OP_ASSIGNMENT::ADDASSIGN[1] == next) return 2;
        if(ArkScript::OP_ASSIGNMENT::SUBASSIGN[0] == letter && ArkScript::OP_ASSIGNMENT::SUBASSIGN[1] == next) return 2;
        if(ArkScript::OP_ASSIGNMENT::DIVASSIGN[0] == letter && ArkScript::OP_ASSIGNMENT::DIVASSIGN[1] == next) return 2;
        if(ArkScript::OP_ASSIGNMENT::MULTASSIGN[0] == letter && ArkScript::OP_ASSIGNMENT::MULTASSIGN[1] == next) return 2;
    }

    if(letter == ArkScript::OP_ASSIGNMENT::ASSIGN[0] && next != ArkScript::OP_ASSIGNMENT::ASSIGN[0]) return 1;

    return 0;
}

bool ArkScript::Lexer::IsIdentifier(const std::string& target)
{
    if (target.empty()) return false;

    char first = target.front();
    bool isValidFirst = (first >= 'a' && first <= 'z') || 
                        (first >= 'A' && first <= 'Z') || 
                        (first == '_');
    
    if (!isValidFirst) return false;

    for (size_t i = 1; i < target.size(); ++i)
    {
        char c = target[i];
        bool isValidChar = (c >= 'a' && c <= 'z') || 
                           (c >= 'A' && c <= 'Z') || 
                           (c >= '0' && c <= '9') || 
                           (c == '_');
        
        if (!isValidChar) return false;
    }

    return true;
}