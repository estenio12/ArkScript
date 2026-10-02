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

                auto res = this->symbol_table->Create(sym);

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

            case ArkScript::Ast::NodeType::MODULE_READONLY_DECL:
            {
                auto* local_stmt = static_cast<ArkScript::Ast::ModuleReadonlyDeclNode*>(stmt.get());

                ArkScript::Symbol sym;
                sym.name = local_stmt->name;
                sym.kind = ArkScript::SymbolKind::VARIABLE;
                sym.is_public = local_stmt->is_public;
                sym.is_readonly = true;
                sym.type = local_stmt->type;

                auto res = this->symbol_table->Create(sym);

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

            case ArkScript::Ast::NodeType::MODULE_READONLY_DECL:
            {
                auto* const_node = static_cast<ArkScript::Ast::ModuleReadonlyDeclNode*>(stmt.get());
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

    for (const auto& param : fun_decl.parameters)
    {
        auto sym = ArkScript::Symbol();
        sym.name = param->name;
        sym.kind = ArkScript::SymbolKind::VARIABLE;
        sym.type = param->type;

        auto res = this->symbol_table->Create(sym);
        if (res == ArkScript::SymbolError::REDECLARATION_SAME_SCOPE)
        {
            // this->ThrowError(param->GetLocation(), "Redeclaration of parameter '" + param->name + "'.");
        }
    }

    // 3. Analisa as instruções do corpo da função (BlockNode / Stmts)
    // ...

    this->symbol_table->PopScope();
}
