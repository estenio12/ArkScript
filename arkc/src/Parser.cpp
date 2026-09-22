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
    auto tk_module = this->ExpectTokenContent(ArkScript::KEYWORDS::TMODULE, "Expected 'module' declaration at the beginning of the file.");
    
    auto module = std::make_unique<ArkScript::Ast::ModuleDeclNode>(tk_module);
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
            mod_stmt->stmts.push_back(this->ParseModuleReadonlyDecl(is_public));
        }
        else if (token.content == ArkScript::KEYWORDS::TFUN)
        {
            mod_stmt->stmts.push_back(this->ParseFunDecl(is_public));
        }
        else
        {
            this->ThrowParserError(token, "Expected 'readonly' or 'fun' declaration in module body.");
        }
    }

    return mod_stmt;
}

std::unique_ptr<ArkScript::Ast::VarDeclNode> ArkScript::Parser::ParseVarDecl()
{
    auto tk_decl = this->tokens->Consume();
    auto var_decl = std::make_unique<ArkScript::Ast::VarDeclNode>(tk_decl);

    if(tk_decl.content == ArkScript::KEYWORDS::TCONST ||
       tk_decl.content == ArkScript::KEYWORDS::TVAR   )
    {
        var_decl->is_constant = tk_decl.content == ArkScript::KEYWORDS::TCONST;
    }
    else
    {
        this->ThrowParserError(tk_decl, "Expected 'const' or 'var keyword to start declaration.");
    }

    auto tk_identifier = this->ExpectTokenType(ArkScript::TokenType::IDENTIFIER, "Expected identifier after keywords 'const', 'var' or 'readonly'.");
    var_decl->name = tk_identifier.content;

    this->ExpectTokenContent(ArkScript::DELIMITER::COLON, "Expected ':' after identifier.");
    
    auto tk_type = this->ExpectTokenType(ArkScript::TokenType::KEYWORD, "Expected a type definition after identifier.");
    var_decl->native_type = tk_type.content;

    if(this->tokens->Peek().content == ArkScript::OP_ASSIGNMENT::ASSIGN)
    {
        // Consume '='.
        this->tokens->Consume(); 
        var_decl->initializer = this->ParseExpression();
    }
    else
    {
        if(var_decl->is_constant)
        {
            this->ThrowParserError(this->tokens->Peek(), "Constant definition must have an atribuition.");
        }
    }

    this->ExpectTokenContent(ArkScript::DELIMITER::SEMICOLON, "Expected ';' at the end of declaration.");

    return var_decl;
}

std::unique_ptr<ArkScript::Ast::FunDeclNode> ArkScript::Parser::ParseFunDecl(bool is_public)
{
    auto tk_fun = this->ExpectTokenContent(ArkScript::KEYWORDS::TFUN, "Expected keyword 'fun' to start function declaration.");
    
    auto fun_decl = std::make_unique<ArkScript::Ast::FunDeclNode>(tk_fun);
    fun_decl->is_public = is_public;

    auto tk_identifier = this->ExpectTokenType(ArkScript::TokenType::IDENTIFIER, "Expected function name after 'fun'.");
    fun_decl->name = tk_identifier.content;

    this->ExpectTokenContent(ArkScript::DELIMITER::LPARAN, "Expected '(' after function identifier.");
    
    fun_decl->parameters = this->ParseParameterList();
    
    this->ExpectTokenContent(ArkScript::DELIMITER::RPARAN, "Expected ')' after parameter list.");
    this->ExpectTokenContent(ArkScript::DELIMITER::ARROW, "Expected '->' after closing parameter list.");

    auto tk_type = this->ExpectTokenType(ArkScript::TokenType::KEYWORD, "Expected a return type definition after '->'.");
    fun_decl->return_type = tk_type.content;
    fun_decl->body = this->ParseBlockScope();
  
    return fun_decl;
}

std::unique_ptr<ArkScript::Ast::ModuleReadonlyDeclNode> ArkScript::Parser::ParseModuleReadonlyDecl(bool is_public)
{
    auto tk_readonly = this->ExpectTokenContent(ArkScript::KEYWORDS::TREADONLY, "Expected 'readonly' keyword to start declaration.");
    auto readonly_decl = std::make_unique<ArkScript::Ast::ModuleReadonlyDeclNode>(tk_readonly);
    readonly_decl->is_public = is_public;

    auto tk_identifier = this->ExpectTokenType(ArkScript::TokenType::IDENTIFIER, "Expected identifier after 'readonly'.");
    readonly_decl->name = tk_identifier.content;

    this->ExpectTokenContent(ArkScript::DELIMITER::COLON, "Expected ':' after identifier.");
    
    auto tk_type = this->ExpectTokenType(ArkScript::TokenType::KEYWORD, "Expected a type definition after identifier.");
    readonly_decl->native_type = tk_type.content;

    this->ExpectTokenContent(ArkScript::OP_ASSIGNMENT::ASSIGN, "Readonly declaration must be initialized with an assignment '='.");

    readonly_decl->initializer = this->ParseExpression();

    this->ExpectTokenContent(ArkScript::DELIMITER::SEMICOLON, "Expected ';' at the end of declaration.");

    return readonly_decl;
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

            if (this->tokens->Peek().content == ArkScript::DELIMITER::RPARAN)
            {
                this->ThrowParserError(this->tokens->Peek(), "Unexpected trailing comma in parameter list.");
            }
        }
        else
        {
            break;
        }
    }

    return parameters;
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
    auto token = this->tokens->Peek();

    if(token.content == ArkScript::KEYWORDS::TCONST ||
       token.content == ArkScript::KEYWORDS::TVAR   )
    {
        return this->ParseVarDecl();
    }
    else if(token.content == ArkScript::KEYWORDS::TRETURN)
    {
        return this->ParseReturnStmt();
    }
    else if(token.type == ArkScript::TokenType::IDENTIFIER)
    {
        if(this->tokens->Peek(1).content == ArkScript::OP_ASSIGNMENT::ASSIGN)
        {
            return this->ParseAssignStmt();
        }
        else if(this->tokens->Peek(1).content == ArkScript::DELIMITER::LPARAN)
        {
            return this->ParseCallStmt();
        }
    }

    this->ThrowParserError(token, "Unexpected '"+ token.content+ "' inside statement block.");
}

std::unique_ptr<ArkScript::Ast::ReturnStmtNode> ArkScript::Parser::ParseReturnStmt()
{
    auto tk_ret = this->ExpectTokenContent(ArkScript::KEYWORDS::TRETURN, "Expected 'return' keyword to start return statement.");
    auto return_stmt = std::make_unique<ArkScript::Ast::ReturnStmtNode>(tk_ret);
    
    if (this->tokens->Peek().content != ArkScript::DELIMITER::SEMICOLON)
    {
        return_stmt->expression = this->ParseExpression();
    }

    this->ExpectTokenContent(ArkScript::DELIMITER::SEMICOLON, "Expected ';' at the end of return statement.");
    return return_stmt;
}

std::unique_ptr<ArkScript::Ast::AssignStmtNode> ArkScript::Parser::ParseAssignStmt()
{
    auto tk_identifier = this->ExpectTokenType(ArkScript::TokenType::IDENTIFIER, "Expected an identifier target for assignment.");
    auto assign_stmt = std::make_unique<ArkScript::Ast::AssignStmtNode>(tk_identifier);
    
    this->ExpectTokenContent(ArkScript::OP_ASSIGNMENT::ASSIGN, "Expected '=' after identifier.");
    
    assign_stmt->expression = this->ParseExpression();

    this->ExpectTokenContent(ArkScript::DELIMITER::SEMICOLON, "Expected ';' at the end of assignment statement.");

    return assign_stmt;
}

std::unique_ptr<ArkScript::Ast::CallStmtNode> ArkScript::Parser::ParseCallStmt()
{
    auto call_stmt = std::make_unique<ArkScript::Ast::CallStmtNode>(this->tokens->Peek());
    call_stmt->call = this->ParseFunCall();
    this->ExpectTokenContent(ArkScript::DELIMITER::SEMICOLON, "Expected ';' at the end of function call statement.");
    return call_stmt;
}

std::unique_ptr<ArkScript::Ast::FunCallNode> ArkScript::Parser::ParseFunCall()
{
    auto tk_identifier = this->ExpectTokenType(ArkScript::TokenType::IDENTIFIER, "Expected function name identifier in call expression.");
    auto fun_call = std::make_unique<ArkScript::Ast::FunCallNode>(tk_identifier);

    this->ExpectTokenContent(ArkScript::DELIMITER::LPARAN, "Expected '(' after identifier.");

    if(this->tokens->Peek().content != ArkScript::DELIMITER::RPARAN)
    {
        fun_call->argument = this->ParseArgumentList();
    }

    this->ExpectTokenContent(ArkScript::DELIMITER::RPARAN, "Expected ')' to close argument list.");
    
    return fun_call;
}

std::unique_ptr<ArkScript::Ast::ArgumentList> ArkScript::Parser::ParseArgumentList()
{
    auto arg_list = std::make_unique<ArkScript::Ast::ArgumentList>(this->tokens->Peek());

    while (true)
    {
        arg_list->arguments.push_back(this->ParseExpression());

        if (this->tokens->Peek().content == ArkScript::DELIMITER::COMMA)
        {
            this->tokens->Consume();
            
            if (this->tokens->Peek().content == ArkScript::DELIMITER::RPARAN)
            {
                this->ThrowParserError(this->tokens->Peek(), "Unexpected trailing comma in argument list.");
            }
        }
        else
        {
            break;
        }
    }

    return arg_list;
}

std::unique_ptr<ArkScript::Ast::ExpressionNode> ArkScript::Parser::ParseExpression(Precedence precedence)
{
    auto left = this->ParsePrefixExpression();
    if (!left) return nullptr;

    while (precedence < this->GetTokenPrecedence(this->tokens->Peek()))
    {
        auto op_token = this->tokens->Peek();

        if (op_token.content == ArkScript::OP_ARITHMETIC::ADD ||
            op_token.content == ArkScript::OP_ARITHMETIC::SUB ||
            op_token.content == ArkScript::OP_ARITHMETIC::MUL ||
            op_token.content == ArkScript::OP_ARITHMETIC::DIV )
        {
            left = this->ParseInfixExpression(std::move(left));
        }
        else
        {
            break;
        }
    }

    return left;
}

std::unique_ptr<ArkScript::Ast::ExpressionNode> ArkScript::Parser::ParsePrefixExpression()
{
    const auto& token = this->tokens->Peek();

    // Sub-expression '(' <expression> ')'
    if (token.content == ArkScript::DELIMITER::LPARAN)
    {
        this->tokens->Consume(); // Consume '('

        auto expr = this->ParseExpression(Precedence::LOWEST);

        this->ExpectTokenContent(ArkScript::DELIMITER::RPARAN, "Expected ')' to close expression.");
        return expr;
    }

    // Function calling or variables/constants
    if (token.type == ArkScript::TokenType::IDENTIFIER)
    {
        if (this->tokens->Peek(1).content == ArkScript::DELIMITER::LPARAN)
        {
            return this->ParseFunCall();
        }

        this->tokens->Consume();
        return std::make_unique<ArkScript::Ast::IdentifierNode>(token);
    }

    // Literals Primitives
    if (token.type == ArkScript::TokenType::LITERAL_INT    ||
        token.type == ArkScript::TokenType::LITERAL_FLOAT  ||
        token.type == ArkScript::TokenType::LITERAL_STRING ||
        token.type == ArkScript::TokenType::LITERAL_BOOL   ||
        token.type == ArkScript::TokenType::LITERAL_CHAR   )
    {
        this->tokens->Consume();
        return std::make_unique<ArkScript::Ast::LiteralNode>(token);
    }

    this->ThrowParserError(token, "Unexpected token '" + token.content + "' in expression.");
    return nullptr;
}

std::unique_ptr<ArkScript::Ast::ExpressionNode> ArkScript::Parser::ParseInfixExpression(
    std::unique_ptr<ArkScript::Ast::ExpressionNode> left)
{
    auto op_token = this->tokens->Consume(); // Consume +, -, *, /
    
    auto binary_node = std::make_unique<ArkScript::Ast::BinaryExprNode>(op_token);
    binary_node->op = op_token.content;
    binary_node->left = std::move(left);

    auto current_prec = this->GetTokenPrecedence(op_token);

    binary_node->right = this->ParseExpression(current_prec);

    return binary_node;
}




