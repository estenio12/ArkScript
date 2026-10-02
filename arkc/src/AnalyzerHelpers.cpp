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
#include "Output.hpp"

[[noreturn]] void ArkScript::Analyzer::ThrowError(const ArkScript::Ast::SourceLocation& src_loc, const std::string& msg)
{
    auto error = this->GenerateFilePathInfoBySourceLocation(src_loc);
    Output::PrintError(error + ": ", false, false);
    Output::Print(msg);
    std::exit(EXIT_FAILURE);
}

std::string ArkScript::Analyzer::GenerateFilePathInfoBySourceLocation(const ArkScript::Ast::SourceLocation& src_loc) const
{
    std::string buffer = this->ast->source_file_path;
    buffer += ":" + std::to_string(src_loc.line) + 
              ":" + std::to_string(src_loc.col);
    return buffer;
}

