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
#include <string>
#include <memory>
#include "Global.hpp"
#include "Lexer.hpp"
#include "Parser.hpp"
#include "Analyzer.hpp"

namespace ArkScript
{
    const std::shared_ptr<const ArkScript::SymbolTable> Compiler(const std::string& source_file)
    {
        auto tokens = std::make_unique<ArkScript::Lexer>(source_file)->Tokenize();
        auto ast = std::make_unique<ArkScript::Parser>(std::move(tokens))->Parse();
        auto sym = std::make_shared<ArkScript::Analyzer>(ast)->Analyze();
        // TODO: auto ir_raw = std::make_unique<ArkScript::IRGenerator>(ast, sym)->Generate();
        // TODO: auto ir_opt = std::make_unique<ArkScript::IROptmizer>(std::move(ir_raw))->Run();

        // TODO: Gerar o arquivo .arki para source_file passado utilizando sym e a função GetPublicSymbols().
        // TODO: Gerar o arquivo .arko para source_file usando o ir_opt.

        // TODO: Salvar os aquivos usando o nome do módulo para que seja fácil encontrá-lo posteriormente.

        return sym;
    }
}