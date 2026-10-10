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
#include "Definitions.hpp"

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
                sym.kind = ArkScript::SymbolKind::CONSTANT;
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
                auto* const_node = static_cast<ArkScript::Ast::ReadonlyDeclNode*>(stmt.get());
                auto ctx = ExprContext();
                ctx.expected_type = const_node->type;
                ctx.is_const_eval = true;
                this->AnalyzeExpression(*const_node->initializer, ctx);
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

    auto ctx = ArkScript::BlockScopeContext();
    ctx.expected_type = fun_decl.return_type;
    ctx.kind = ArkScript::SymbolKind::FUNCTION;
    this->AnalyzeBlockScope(*fun_decl.body, ctx);

    this->symbol_table->PopScope();
}

void ArkScript::Analyzer::AnalyzeBlockScope(ArkScript::Ast::BlockScopeNode& body, const BlockScopeContext& ctx)
{
    this->symbol_table->PushScope();

    for(const auto& stmt : body.stmts)
    {
        this->AnalyzeStmt(*stmt, ctx);
    }

    this->symbol_table->PopScope();
}

void ArkScript::Analyzer::AnalyzeStmt(ArkScript::Ast::StatementNode& stmt, const BlockScopeContext& ctx)
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
            auto* call_node = static_cast<ArkScript::Ast::CallStmtNode*>(&stmt);
            this->AnalyzeFunCall(*call_node->call);
        }
        break;

        case ArkScript::Ast::NodeType::ASSIGN_STMT:
        {
            auto* assign_stmt = static_cast<ArkScript::Ast::AssignStmtNode*>(&stmt);
            this->AnalyzeAssignStmt(*assign_stmt);
        }
        break;

        case ArkScript::Ast::NodeType::RETURN_STMT:
        {
            auto expr_ctx = ArkScript::ExprContext();
            expr_ctx.expected_type = ctx.expected_type;
            auto* return_stmt = static_cast<ArkScript::Ast::ReturnStmtNode*>(&stmt);
            this->AnalyzeExpression(*return_stmt->expression, expr_ctx);
        }
        break;

        default:
        {
            this->ThrowError(stmt.GetLocation(), "Internal error: unhandled statement node type.");
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
            auto fun_sym = this->AnalyzeFunCall(*call_node);

            expr.InferredType = fun_sym.type;
            if (!ctx.expected_type.empty() && expr.InferredType != ctx.expected_type)
            {
                this->ThrowError(expr.GetLocation(), 
                    "function '" + call_node->name + 
                    "' returns '" + expr.InferredType + "', but expected '" + ctx.expected_type + "'.");
            }
        }
        break;

        case ArkScript::Ast::NodeType::BIN_OP:
        {
            auto* bin_node = static_cast<ArkScript::Ast::BinaryOpNode*>(&expr);
            auto sub_ctx = ctx;

            bool is_comparison = (bin_node->op == ArkScript::OP_COMPARISON::EQ   || 
                                  bin_node->op == ArkScript::OP_COMPARISON::DIFF || 
                                  bin_node->op == ArkScript::OP_COMPARISON::LT   || 
                                  bin_node->op == ArkScript::OP_COMPARISON::GT   || 
                                  bin_node->op == ArkScript::OP_COMPARISON::LTEQ || 
                                  bin_node->op == ArkScript::OP_COMPARISON::GTEQ );

            if(is_comparison)
            {
                sub_ctx.expected_type = "";
            }

            this->AnalyzeExpression(*bin_node->left, sub_ctx);
            this->AnalyzeExpression(*bin_node->right, sub_ctx);
        }
        break;

        case ArkScript::Ast::NodeType::LITERAL_BOOL:
        case ArkScript::Ast::NodeType::LITERAL_CHAR:
        case ArkScript::Ast::NodeType::LITERAL_FLOAT:
        case ArkScript::Ast::NodeType::LITERAL_INT:
        case ArkScript::Ast::NodeType::LITERAL_STRING:
        {
            std::string natural_type = "";
            switch (expr.type)
            {
                case ArkScript::Ast::NodeType::LITERAL_BOOL:   natural_type = ArkScript::KEYWORDS::TBOOL; break;
                case ArkScript::Ast::NodeType::LITERAL_CHAR:   natural_type = ArkScript::KEYWORDS::TCHAR; break;
                case ArkScript::Ast::NodeType::LITERAL_STRING: natural_type = ArkScript::KEYWORDS::TSTR;  break;
                case ArkScript::Ast::NodeType::LITERAL_INT:    natural_type = ArkScript::KEYWORDS::TI32;  break; // Padrão i32 para inteiros
                case ArkScript::Ast::NodeType::LITERAL_FLOAT:  natural_type = ArkScript::KEYWORDS::TF32;  break; // Padrão f32 para floats
                default: break;
            }

            if(ctx.expected_type.empty())
            {
                expr.InferredType = natural_type;
            }
            else 
            {
                bool is_compatible = false;
                std::string actual_type = "";

                switch (expr.type)
                {
                    case ArkScript::Ast::NodeType::LITERAL_BOOL:
                    {
                        actual_type = ArkScript::KEYWORDS::TBOOL;
                        is_compatible = (ctx.expected_type == ArkScript::KEYWORDS::TBOOL);
                    }
                    break;

                    case ArkScript::Ast::NodeType::LITERAL_CHAR:
                    {
                        actual_type = ArkScript::KEYWORDS::TCHAR;
                        is_compatible = (ctx.expected_type == ArkScript::KEYWORDS::TCHAR);
                    }
                    break;

                    case ArkScript::Ast::NodeType::LITERAL_STRING:
                    {

                        actual_type = ArkScript::KEYWORDS::TSTR;
                        is_compatible = (ctx.expected_type == ArkScript::KEYWORDS::TSTR);
                    }
                    break;

                    case ArkScript::Ast::NodeType::LITERAL_INT:
                    {
                        actual_type = "integer";
                        is_compatible = (ctx.expected_type == ArkScript::KEYWORDS::TI8   || ctx.expected_type == ArkScript::KEYWORDS::TI16  ||
                                         ctx.expected_type == ArkScript::KEYWORDS::TI32  || ctx.expected_type == ArkScript::KEYWORDS::TI64  ||
                                         ctx.expected_type == ArkScript::KEYWORDS::TUI8  || ctx.expected_type == ArkScript::KEYWORDS::TUI16 ||
                                         ctx.expected_type == ArkScript::KEYWORDS::TUI32 || ctx.expected_type == ArkScript::KEYWORDS::TUI64 );
                    }
                    break;

                    case ArkScript::Ast::NodeType::LITERAL_FLOAT:
                    {
                        actual_type = "float";
                        is_compatible = (ctx.expected_type == ArkScript::KEYWORDS::TF32 || ctx.expected_type == ArkScript::KEYWORDS::TF64);
                    }
                    break;

                    default: break;
                }

                if (!is_compatible)
                {
                    this->ThrowError(expr.GetLocation(), 
                        "Cannot implicitly convert '" + actual_type + "' to '" + ctx.expected_type + "'.");
                }
            }
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

            // In constant context, only constants identifiers are allowed
            if (ctx.is_const_eval && sym->kind != ArkScript::SymbolKind::CONSTANT)
            {
                this->ThrowError(id_node->GetLocation(), 
                    "Cannot reference non-constant variable '" + id_node->name + "' inside a constant expression.");
            }

            if(!ctx.expected_type.empty() && ctx.expected_type != sym->type)
            {
                this->ThrowError(expr.GetLocation(), 
                        "Cannot implicitly convert '" + sym->type + "' to '" + ctx.expected_type + "'.");
            }
        }
        break;

        default:
        {
            this->ThrowError(expr.GetLocation(), "Internal error: unhandled expression node type.");
        }
        break;
    }
}

const ArkScript::Symbol& ArkScript::Analyzer::AnalyzeFunCall(ArkScript::Ast::FunCallNode& fun_call)
{
    const auto* func_sym = this->symbol_table->Search(fun_call.name);
    if (!func_sym)
    {
        this->ThrowError(fun_call.GetLocation(), "Undeclared function '" + fun_call.name + "'.");
    }

    if (func_sym->kind != ArkScript::SymbolKind::FUNCTION)
    {
        this->ThrowError(fun_call.GetLocation(), "'" + fun_call.name + "' is not a function.");
    }

    size_t fun_call_arguments_amount = fun_call.arg_list->arguments.size();
    if (fun_call_arguments_amount != func_sym->args.size())
    {
        this->ThrowError(fun_call.GetLocation(), 
            "Function '" + fun_call.name + "' expects " + 
            std::to_string(func_sym->args.size()) + " argument(s), but " + 
            std::to_string(fun_call_arguments_amount) + " were provided.");
    }

    for (size_t i = 0; i < fun_call_arguments_amount; ++i)
    {
        auto arg_ctx = ArkScript::ExprContext();
        arg_ctx.expected_type = func_sym->args[i];
        arg_ctx.is_const_eval = false;

        this->AnalyzeExpression(*fun_call.arg_list->arguments[i], arg_ctx);
    }

    return *func_sym;
}

void ArkScript::Analyzer::AnalyzeAssignStmt(ArkScript::Ast::AssignStmtNode& stmt)
{
    auto assign_sym = this->symbol_table->Search(stmt.name);

    if(!assign_sym)
    {
        this->ThrowError(stmt.GetLocation(), "Undeclared identifier '" + stmt.name + "'.");
    }
    
    if(assign_sym->kind != ArkScript::SymbolKind::VARIABLE)
    {
        this->ThrowError(stmt.GetLocation(), "Only variables can receive new values.");
    }

    auto ctx = ArkScript::ExprContext();
    ctx.expected_type = assign_sym->type;
    ctx.is_const_eval = false;

    this->AnalyzeExpression(*stmt.expression, ctx);
}


