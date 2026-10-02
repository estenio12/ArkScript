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
#include "SymbolTable.hpp"
#include "Ast.hpp"

namespace ArkScript
{
    class Analyzer
    {
        private:
            std::shared_ptr<ArkScript::Ast::ProgramNode> ast;
            std::shared_ptr<SymbolTable> symbol_table;

        public:
            explicit Analyzer(std::shared_ptr<ArkScript::Ast::ProgramNode> ast)
                : ast(std::move(ast)), symbol_table(std::make_shared<SymbolTable>()) { }
            std::shared_ptr<SymbolTable> Analyze();

        private:
            void MakeHoisting();
            void FullAnalyze();

        // # Auxiliar functions
        private:
            [[noreturn]] void ThrowError(const ArkScript::Ast::SourceLocation& src_loc, const std::string& msg);
            std::string GenerateFilePathInfoBySourceLocation(const ArkScript::Ast::SourceLocation& src_loc) const;
    };
}