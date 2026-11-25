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
#include <string>
#include <vector>
#include "Ast.hpp"
#include "SymbolTable.hpp"
#include "IR.hpp"

namespace ArkScript
{
    class IRGenerator
    {
        private:
            std::shared_ptr<ArkScript::Ast::ProgramNode> ast; 
            std::shared_ptr<ArkScript::SymbolTable> sym;
            std::unique_ptr<Module> current_module;

        public:
            explicit IRGenerator(std::shared_ptr<ArkScript::Ast::ProgramNode> ast, std::shared_ptr<ArkScript::SymbolTable> sym)
                : ast(ast), sym(sym), current_module(std::make_unique<Module>()) { }

            std::unique_ptr<Module> Generate();

        private:
            void VisitProgram(const ArkScript::Ast::ProgramNode& node);
            void VisitFunction(const ArkScript::Ast::FunDeclNode& node);
            void VisitRData(const ArkScript::Ast::ReadonlyDeclNode& node);
            void VisitStatement(const ArkScript::Ast::StatementNode& node, std::vector<Instruction>& field);
            void VisitExpression(const ArkScript::Ast::ExpressionNode& node, std::vector<Instruction>& field);
            void VisitBinaryExpr(const ArkScript::Ast::BinaryOpNode& node, std::vector<Instruction>& field);
            void VisitLiteral(const ArkScript::Ast::LiteralNode& node, std::vector<Instruction>& field);
    };
}
