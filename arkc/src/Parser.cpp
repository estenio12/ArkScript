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
#include "Definitions.hpp"


std::unique_ptr<ArkScript::Ast::ProgramNode> ArkScript::Parser::Parse()
{
    auto program = std::make_unique<ArkScript::Ast::ProgramNode>();
    program->module = this->ParseModuleDecl();
    return program;
}

std::unique_ptr<ArkScript::Ast::ModuleDeclNode> ArkScript::Parser::ParseModuleDecl()
{
    this->ExpectTokenContent(ArkScript::KEYWORDS::TMODULE, "Expected 'module' declaration at the beginning of the file.");
    
    auto module = std::make_unique<ArkScript::Ast::ModuleDeclNode>();
    module->name = this->ExpectTokenType(ArkScript::TokenType::IDENTIFIER, "Expected module name after 'module'.").content;
    
    while (this->tokens->Peek().content == ArkScript::DELIMITER::SCOPEACCESS)
    {
        this->ExpectTokenContent(ArkScript::DELIMITER::SCOPEACCESS, "Expected '::' between module identifiers.");
        auto subName = this->ExpectTokenType(ArkScript::TokenType::IDENTIFIER, "Expected identifier after '::'.");
        module->name += "::" + subName.content;
    }

    this->ExpectTokenContent(ArkScript::DELIMITER::SEMICOLON, "Expected ';' after module declaration.");
    module->stmt = this->ParseModuleStmt();
    return module;
}

std::unique_ptr<ArkScript::Ast::ModuleStmtNode> ArkScript::Parser::ParseModuleStmt()
{
    return std::make_unique<ArkScript::Ast::ModuleStmtNode>();
}