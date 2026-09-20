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

    while (!this->tokens->IsEOF())
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
    
    auto tk_type = this->ExpectTokenType(ArkScript::TokenType::KEYWORD, "Expected a type definition after identifier.");
    var_decl->native_type = tk_type.content;

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
    auto tk_fun = this->ExpectTokenContent(ArkScript::KEYWORDS::TFUN, "Expected keyword 'fun' to start function declaration.");
    
    auto fun_decl = std::make_unique<ArkScript::Ast::FunDeclNode>();
    fun_decl->is_public = is_public;
    fun_decl->SetLocation(tk_fun);

    auto tk_identifier = this->ExpectTokenType(ArkScript::TokenType::IDENTIFIER, "Expected function name after 'fun'.");
    fun_decl->name = tk_identifier.content;

    this->ExpectTokenContent(ArkScript::DELIMITER::LPARAN, "Expected '(' after function identifier.");
    
    fun_decl->parameters = this->ParseParameterList();
    
    this->ExpectTokenContent(ArkScript::DELIMITER::RPARAN, "Expected ')' after parameter list.");
    this->ExpectTokenContent(ArkScript::DELIMITER::ARROW, "Expected '->' after closing parameter list.");

    auto tk_type = this->ExpectTokenType(ArkScript::TokenType::KEYWORD, "Expected a return type definition after '->'.");
    fun_decl->return_type = tk_type.content;
    fun_decl->body = std::move(this->ParseBlockScope());
  
    return fun_decl;
}

std::vector<std::unique_ptr<ArkScript::Ast::ParamNode>> ArkScript::Parser::ParseParameterList()
{
    std::vector<std::unique_ptr<ArkScript::Ast::ParamNode>> parameters;

    if (this->tokens->Peek().content == ArkScript::DELIMITER::RPARAN) return parameters;

    while (true)
    {
        auto tk_param_name = this->ExpectTokenType(ArkScript::TokenType::IDENTIFIER, "Expected parameter name.");

        this->ExpectTokenContent(ArkScript::DELIMITER::COLON, "Expected ':' after parameter name.");

        auto tk_param_type = this->ExpectTokenType(ArkScript::TokenType::KEYWORD, "Expected parameter type.");

        parameters.push_back(std::make_unique<ArkScript::Ast::ParamNode>(
            tk_param_name.content, 
            tk_param_type.content
        ));

        if (this->tokens->Peek().content == ArkScript::DELIMITER::COMMA)
        {
            this->tokens->Consume(); 
        }
        else
        {
            break;
        }
    }

    return parameters;
}

std::unique_ptr<ArkScript::Ast::ExpressionNode> ArkScript::Parser::ParseExpression()
{
    return std::make_unique<ArkScript::Ast::ExpressionNode>(ArkScript::Ast::NodeType::BINARY_EXPR);
}

std::unique_ptr<ArkScript::Ast::BlockScopeNode> ArkScript::Parser::ParseBlockScope()
{
    auto tk_colon = this->ExpectTokenContent(ArkScript::DELIMITER::COLON, "Expected ':' to start block scope.");
    auto block_node = std::make_unique<ArkScript::Ast::BlockScopeNode>(tk_colon);

    while (this->tokens->Peek().content != ArkScript::KEYWORDS::TEND && !this->tokens->IsEOF())
    {
        block_node->stmts.push_back(this->ParseStatement());
    }

    if (this->tokens->IsEOF())
    {
        this->ThrowParserError(this->tokens->Peek(), "Unexpected EOF inside block scope. Expected 'end'.");
    }

    this->ExpectTokenContent(ArkScript::KEYWORDS::TEND, "Expected 'end' keyword to close block scope.");

    return block_node;
}

std::unique_ptr<ArkScript::Ast::StatementNode> ArkScript::Parser::ParseStatement()
{
    return std::make_unique<ArkScript::Ast::StatementNode>(ArkScript::Ast::NodeType::VAR_DECL);
}



