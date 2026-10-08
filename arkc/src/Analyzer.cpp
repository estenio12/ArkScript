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
#include "Analyzer.hpp"

std::shared_ptr<ArkScript::SymbolTable> ArkScript::Analyzer::Analyze()
{
    this->MakeHoisting();
    this->FullAnalyze();
    return this->symbol_table;
}

void ArkScript::Analyzer::MakeHoisting()
{
    const auto& mod_stmts = this->ast->module->stmt->stmts;
    if(mod_stmts.empty()) return;
    
    for(auto& stmt : mod_stmts)
    {
        switch (stmt->type)
        {
            case ArkScript::Ast::NodeType::FUN_DECL:
            {
                auto* local_stmt = static_cast<ArkScript::Ast::FunDeclNode*>(stmt.get());

                auto sym = ArkScript::Symbol();
                sym.name = local_stmt->name;
                sym.kind = ArkScript::SymbolKind::FUNCTION;
                sym.is_public = local_stmt->is_public;
                sym.type = local_stmt->return_type;
                
                for(auto& param : local_stmt->parameters)
                {
                    sym.args.push_back(param->type);
                }

                auto res = this->symbol_table->Create(std::move(sym));

                if(res == ArkScript::SymbolError::REDECLARATION_SAME_SCOPE)
                {
                    this->ThrowError(local_stmt->GetLocation(), "Redeclaration of function '" + local_stmt->name + "' in module scope.");
                }
                else if(res == ArkScript::SymbolError::SHADOWING_PARENT_SCOPE)
                {
                    this->ThrowError(local_stmt->GetLocation(), "Function name '" + local_stmt->name + "' collides with an outer symbol.");
                } 
            }
            break;

            case ArkScript::Ast::NodeType::READONLY_DECL:
            {
                auto* local_stmt = static_cast<ArkScript::Ast::ReadonlyDeclNode*>(stmt.get());

                ArkScript::Symbol sym;
                sym.name = local_stmt->name;
                sym.kind = ArkScript::SymbolKind::VARIABLE;
                sym.is_public = local_stmt->is_public;
                sym.is_readonly = true;
                sym.type = local_stmt->type;

                auto res = this->symbol_table->Create(std::move(sym));

                if(res == ArkScript::SymbolError::REDECLARATION_SAME_SCOPE)
                {
                    this->ThrowError(local_stmt->GetLocation(), "Redeclaration of readonly symbol '" + local_stmt->name + "' in module scope.");
                }
                else if(res == ArkScript::SymbolError::SHADOWING_PARENT_SCOPE)
                {
                    this->ThrowError(local_stmt->GetLocation(), "Readonly symbol '" + local_stmt->name + "' collides with an outer symbol.");
                }
            }
            break;
            
            default:
                this->ThrowError(stmt->GetLocation(), "Unexpected node in module scope.");
            break;
        }
    }
}

void ArkScript::Analyzer::FullAnalyze()
{
    const auto& mod_stmts = this->ast->module->stmt->stmts;
    if(mod_stmts.empty()) return;

    for(const auto& stmt : mod_stmts)
    {
        switch (stmt->type)
        {
            case ArkScript::Ast::NodeType::FUN_DECL:
            {
                auto* fn_node = static_cast<ArkScript::Ast::FunDeclNode*>(stmt.get());
                this->AnalyzeFunction(*fn_node);
            }
            break;

            case ArkScript::Ast::NodeType::READONLY_DECL:
            {
                // auto* const_node = static_cast<ArkScript::Ast::ReadonlyDeclNode*>(stmt.get());
                // TODO: Validar a expressão de inicialização do valor constante
                // this->AnalyzeExpression(*const_node->value_expr);
            }
            break;
            
            default:
                this->ThrowError(stmt->GetLocation(), "Unexpected node in module scope.");
            break;
        }
    }
}

void ArkScript::Analyzer::AnalyzeFunction(ArkScript::Ast::FunDeclNode& fun_decl)
{
    this->symbol_table->PushScope();

    // # Parameters
    for (const auto& param : fun_decl.parameters)
    {
        auto sym = ArkScript::Symbol();
        sym.name = param->name;
        sym.kind = ArkScript::SymbolKind::VARIABLE;
        sym.type = param->type;

        auto res = this->symbol_table->Create(std::move(sym));
        if (res == ArkScript::SymbolError::REDECLARATION_SAME_SCOPE)
        {
            this->ThrowError(fun_decl.GetLocation(), "Redeclaration of parameter '" + param->name + "'.");
        }
    }

    this->AnalyzeBlockScope(*fun_decl.body);

    this->symbol_table->PopScope();
}

void ArkScript::Analyzer::AnalyzeBlockScope(ArkScript::Ast::BlockScopeNode& body)
{
    this->symbol_table->PushScope();

    for(const auto& stmt : body.stmts)
    {
        this->AnalyzeStmt(*stmt);
    }

    this->symbol_table->PopScope();
}

void ArkScript::Analyzer::AnalyzeStmt(ArkScript::Ast::StatementNode& stmt)
{
    switch(stmt.type)
    {
        case ArkScript::Ast::NodeType::VAR_DECL:
        {
            auto* var_decl = static_cast<ArkScript::Ast::VarDeclNode*>(&stmt);
            this->AnalyzeVarDecl(*var_decl);
        }
        break;

        case ArkScript::Ast::NodeType::CALL_STMT:
        {
            // TODO
        }
        break;

        case ArkScript::Ast::NodeType::ASSIGN_STMT:
        {
            // TODO
        }
        break;

        case ArkScript::Ast::NodeType::RETURN_STMT:
        {
            // TODO
        }
        break;
    }
}

void ArkScript::Analyzer::AnalyzeVarDecl(ArkScript::Ast::VarDeclNode& var_decl)
{
    auto sym = ArkScript::Symbol();
    sym.name = var_decl.name;
    sym.kind = var_decl.is_constant ? ArkScript::SymbolKind::CONSTANT : ArkScript::SymbolKind::VARIABLE;
    sym.type = var_decl.type;

    auto res = this->symbol_table->Create(std::move(sym));
    if (res == ArkScript::SymbolError::REDECLARATION_SAME_SCOPE)
    {
        this->ThrowError(var_decl.GetLocation(), "Redeclaration of '" + var_decl.name + "'.");
    }
    else if (res == ArkScript::SymbolError::SHADOWING_PARENT_SCOPE)
    {
        this->ThrowError(var_decl.GetLocation(), "Declaration of '" + var_decl.name + "' collides with an outer symbol.");
    }

    if (var_decl.initializer)
    {
        auto ctx = ArkScript::ExprContext();
        ctx.expected_type = var_decl.type;
        ctx.is_const_eval = var_decl.is_constant;

        this->AnalyzeExpression(*var_decl.initializer, ctx);
    }
    else
    {   
        if (var_decl.is_constant)
        {
            this->ThrowError(var_decl.GetLocation(), "Constant '" + var_decl.name + "' must be initialized upon declaration.");
        }
    }
}

void ArkScript::Analyzer::AnalyzeExpression(ArkScript::Ast::ExpressionNode& expr, const ArkScript::ExprContext& ctx)
{
    switch (expr.type)
    {
        case ArkScript::Ast::NodeType::FUN_CALL:
        {
            if (ctx.is_const_eval)
            {
                this->ThrowError(expr.GetLocation(), "Function calls are not allowed in compile-time constant expressions.");
            }

            auto* call_node = static_cast<ArkScript::Ast::FunCallNode*>(&expr);
            //TODO: this->AnalyzeCallExpr(*call_node, ctx);
        }
        break;

        case ArkScript::Ast::NodeType::BINARY_EXPR:
        {
            auto* bin_node = static_cast<ArkScript::Ast::BinaryOpNode*>(&expr);
            
            // Repassa o mesmo contexto para os operandos esquerdo e direito
            this->AnalyzeExpression(*bin_node->left, ctx);
            this->AnalyzeExpression(*bin_node->right, ctx);

            // TODO: Aqui você também pode checar se ctx.expected_type bate com o resultado do operador!
        }
        break;

        case ArkScript::Ast::NodeType::LITERAL:
        {
            // Valida se o tipo do literal bate com ctx.expected_type (se informado)
        }
        break;

        case ArkScript::Ast::NodeType::IDENTIFIER:
        {
            auto* id_node = static_cast<ArkScript::Ast::IdentifierNode*>(&expr);
            const auto* sym = this->symbol_table->Search(id_node->name);

            if (!sym)
            {
                this->ThrowError(id_node->GetLocation(), "Undeclared identifier '" + id_node->name + "'.");
            }

            // Se for um contexto de const_eval, só aceita identificadores que também sejam CONSTANTES
            if (ctx.is_const_eval && sym->kind != ArkScript::SymbolKind::CONSTANT)
            {
                this->ThrowError(id_node->GetLocation(), 
                    "Cannot reference non-constant variable '" + id_node->name + "' inside a constant expression.");
            }
        }
        break;
    }
}