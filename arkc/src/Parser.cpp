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
    program->module = std::move(this->ParseModuleDecl());
    return program;
}

std::unique_ptr<ArkScript::Ast::ModuleDeclNode> ArkScript::Parser::ParseModuleDecl()
{
    auto tk_module = this->ExpectTokenContent(ArkScript::KEYWORDS::TMODULE, "Expected 'module' declaration at the beginning of the file.");
    
    auto module = std::make_unique<ArkScript::Ast::ModuleDeclNode>();
    module->name = this->ExpectTokenType(ArkScript::TokenType::IDENTIFIER, "Expected module name after 'module'.").content;
    module->SetLocation(tk_module);
    
    while (this->tokens->Peek().content == ArkScript::DELIMITER::SCOPEACCESS)
    {
        this->ExpectTokenContent(ArkScript::DELIMITER::SCOPEACCESS, "Expected '::' between module identifiers.");
        auto subName = this->ExpectTokenType(ArkScript::TokenType::IDENTIFIER, "Expected identifier after '::'.");
        module->name += "::" + subName.content;
    }

    this->ExpectTokenContent(ArkScript::DELIMITER::SEMICOLON, "Expected ';' after module declaration.");
    module->stmt = std::move(this->ParseModuleStmt());
    return module;
}

std::unique_ptr<ArkScript::Ast::ModuleStmtNode> ArkScript::Parser::ParseModuleStmt()
{
    auto mod_stmt = std::make_unique<ArkScript::Ast::ModuleStmtNode>();

    while (!this->tokens->IsAtEnd())
    {
        bool is_public = false;

        if (this->tokens->Peek().content == ArkScript::KEYWORDS::TPUB)
        {
            this->tokens->Consume();
            is_public = true;
        }

        auto token = this->tokens->Peek();

        if (token.content == ArkScript::KEYWORDS::TREADONLY)
        {
            mod_stmt->stmts.push_back(std::move(this->ParseVarDecl(is_public)));
        }
        else if (token.content == ArkScript::KEYWORDS::TFUN)
        {
            mod_stmt->stmts.push_back(std::move(this->ParseFunDecl(is_public)));
        }
        else
        {
            this->ThrowParserError(token, "Expected 'readonly' or 'fun' declaration in module body.");
        }
    }

    return mod_stmt;
}

std::unique_ptr<ArkScript::Ast::VarDeclNode> ArkScript::Parser::ParseVarDecl(bool is_public)
{
    auto var_decl = std::make_unique<ArkScript::Ast::VarDeclNode>();
    var_decl->is_public = is_public;
    
    auto tk_decl = this->tokens->Consume();
    var_decl->SetLocation(tk_decl);

    if(tk_decl.content == ArkScript::KEYWORDS::TREADONLY)
    {
        var_decl->kind = ArkScript::Ast::BindingKind::READONLY;
    }
    else if(tk_decl.content == ArkScript::KEYWORDS::TCONST)
    {
        var_decl->kind = ArkScript::Ast::BindingKind::CONST;
    }
    else if(tk_decl.content == ArkScript::KEYWORDS::TVAR)
    {
        var_decl->kind = ArkScript::Ast::BindingKind::VAR;
    }
    else
    {
        this->ThrowParserError(tk_decl, "Expected 'const', 'var' or 'readonly' keyword to start declaration.");
    }

    auto tk_identifier = this->ExpectTokenType(ArkScript::TokenType::IDENTIFIER, "Expected identifier after keywords 'const', 'var' or 'readonly'.");
    var_decl->name = tk_identifier.content;

    this->ExpectTokenContent(ArkScript::DELIMITER::COLON, "Expected ':' after identifier.");
    
    auto tk_type = this->tokens->Consume();
    if(tk_type.type == ArkScript::TokenType::KEYWORD)
    {
        var_decl->native_type = tk_type.content;
    }
    else
    {
        this->ThrowParserError(tk_type, "Expected a type definition after identifier.");
    }

    if(this->tokens->Peek().content == ArkScript::OP_ASSIGNMENT::ASSIGN)
    {
        // Consome '='.
        this->tokens->Consume(); 
        var_decl->initializer = std::move(this->ParseExpression());
    }
    else
    {
        if(var_decl->kind == ArkScript::Ast::BindingKind::CONST)
        {
            this->ThrowParserError(this->tokens->Peek(), "Constant definition must have an assignment.");
        }
        
        if(var_decl->kind == ArkScript::Ast::BindingKind::READONLY)
        {
            this->ThrowParserError(this->tokens->Peek(), "Readonly definition must have an assignment.");
        }
    }

    this->ExpectTokenContent(ArkScript::DELIMITER::SEMICOLON, "Expected ';' at the end of declaration.");

    return var_decl;
}

std::unique_ptr<ArkScript::Ast::FunDeclNode> ArkScript::Parser::ParseFunDecl(bool is_public)
{
    return std::make_unique<ArkScript::Ast::FunDeclNode>();
}

std::unique_ptr<ArkScript::Ast::ExpressionNode> ArkScript::Parser::ParseExpression()
{
    return std::make_unique<ArkScript::Ast::ExpressionNode>(ArkScript::Ast::NodeType::BINARY_EXPR);
}


