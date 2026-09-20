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
#include <memory>
#include "TokenManager.hpp"
#include "Ast.hpp"

namespace ArkScript
{
    class Parser
    {
        private:
            std::unique_ptr<ArkScript::TokenManager> tokens;

        public:
            Parser(std::unique_ptr<ArkScript::TokenManager> tokens) : tokens(std::move(tokens)) {}
            ~Parser() = default;

        public:
            std::unique_ptr<ArkScript::Ast::ProgramNode> Parse();
        
        private:
            const ArkScript::Token ExpectTokenType(const ArkScript::TokenType& type, const std::string& message);
            const ArkScript::Token ExpectTokenContent(const std::string& content, const std::string& message);
            std::string GenerateFilePathInfoByToken(const ArkScript::Token& target);
            [[noreturn]] void ThrowParserError(const ArkScript::Token& token, std::string message);

        private:
            std::unique_ptr<ArkScript::Ast::ModuleDeclNode> ParseModuleDecl();
            std::unique_ptr<ArkScript::Ast::ModuleStmtNode> ParseModuleStmt();
            std::unique_ptr<ArkScript::Ast::VarDeclNode> ParseVarDecl(bool is_public = false);
            std::unique_ptr<ArkScript::Ast::FunDeclNode> ParseFunDecl(bool is_public = false);
            std::unique_ptr<ArkScript::Ast::ExpressionNode> ParseExpression();
    };
}