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
#include <memory>
#include "Parser.hpp"
#include "Ast.hpp"
#include "Output.hpp"
#include "Definitions.hpp"

const ArkScript::Token ArkScript::Parser::ExpectTokenType(const ArkScript::TokenType& expected, const std::string& message)
{
    auto token = this->tokens->Consume();
    if(token.type == expected) return token;

    this->ThrowParserError(token, message);
}

const ArkScript::Token ArkScript::Parser::ExpectTokenContent(const std::string& expected, const std::string& message)
{
    auto token = this->tokens->Consume();
    if(token.content == expected) return token;

    this->ThrowParserError(token, message);
}

std::string ArkScript::Parser::GenerateFilePathInfoByToken(const ArkScript::Token& token)
{
    std::string buffer = this->tokens->GetFilePath();
    buffer += "(Ln: " + std::to_string(token.line) + 
              ", Col: " + std::to_string(token.col) + 
              ", Len: " + std::to_string(token.content.length()) + ")";
    return buffer;
}

[[noreturn]] void ArkScript::Parser::ThrowParserError(const ArkScript::Token& token, std::string message)
{
    auto module_error = this->GenerateFilePathInfoByToken(token);
    Output::PrintError(module_error + ": ", false, false);
    Output::Print(message);
    exit(1);
}

ArkScript::Precedence ArkScript::Parser::GetTokenPrecedence(const ArkScript::Token& token) const
{
    if (token.content == ArkScript::OP_ARITHMETIC::ADD || 
        token.content == ArkScript::OP_ARITHMETIC::SUB)
    {
        return ArkScript::Precedence::SUM;
    }

    if (token.content == ArkScript::OP_ARITHMETIC::MUL || 
        token.content == ArkScript::OP_ARITHMETIC::DIV)
    {
        return ArkScript::Precedence::PRODUCT;
    }

    if (token.content == ArkScript::DELIMITER::SCOPEACCESS)
    {
        return ArkScript::Precedence::SCOPE;
    }

    return ArkScript::Precedence::LOWEST;
}