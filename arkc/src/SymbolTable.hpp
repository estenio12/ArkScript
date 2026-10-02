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

#include <string>
#include <cstdint>
#include <vector>

namespace ArkScript
{
    enum class SymbolKind: uint8_t
    {
        FUNCTION,
        VARIABLE,
        MODULE
    };

    struct Symbol
    {
        std::string name;
        std::string type;
        SymbolKind kind;
        uint32_t depth = 0;
        std::vector<std::string> args;
        bool is_public = false;
        bool is_readonly = false;
    };

    enum class SymbolError
    {
        SUCCESS,
        REDECLARATION_SAME_SCOPE,
        SHADOWING_PARENT_SCOPE
    };

    class SymbolTable 
    {
        private:
            std::vector<Symbol> symbols;
            uint32_t current_depth = 0;
            std::string module_name;

        public:
            uint32_t GetCurrentDepth() const { return current_depth; }
            void PushScope() { current_depth++; }
            void PopScope() 
            { 
                while (!symbols.empty() && symbols.back().depth == current_depth) 
                {
                    symbols.pop_back();
                }
                if (current_depth > 0) current_depth--;
            }

            SymbolError Create(Symbol sym) 
            {
                sym.depth = current_depth;

                for (auto it = symbols.rbegin(); it != symbols.rend(); ++it) 
                {
                    if (it->name == sym.name) 
                    {
                        if(it->depth == sym.depth)
                        {
                            return SymbolError::REDECLARATION_SAME_SCOPE;
                        }
                        
                        return SymbolError::SHADOWING_PARENT_SCOPE;
                    }
                }

                symbols.push_back(std::move(sym));
                return SymbolError::SUCCESS;
            }

            const Symbol* Search(const std::string& name) const 
            {
                for (auto it = symbols.rbegin(); it != symbols.rend(); ++it) 
                {
                    if (it->name == name) 
                    {
                        return &(*it);
                    }
                }
                return nullptr;
            }

            // Get public symbols for generate file .arki
            std::vector<Symbol> GetPublicSymbols() const 
            {
                std::vector<Symbol> pub_symbols;
                for (const auto& sym : symbols) 
                {
                    if (sym.is_public && sym.depth == 0) 
                    {
                        pub_symbols.push_back(sym);
                    }
                }
                return pub_symbols;
            }
    };
}