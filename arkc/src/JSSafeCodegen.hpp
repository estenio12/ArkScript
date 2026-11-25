/*
 * Copyright 2026 ArkScript Authors
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 *     http://apache.org
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#pragma once
#include <string>
#include <vector>
#include <memory>
#include "IR.hpp"

namespace ArkScript
{
    class JSSafeCodegen
    {
        private:
            std::string js_buffer;
            std::vector<std::string> compile_stack;

        public:
            JSSafeCodegen() = default;
            std::string Transpile(const Module& module);

        private:
            void EmitHeader(const std::string& module_name);
            void ProcessInstructions(const std::vector<Instruction>& instructions);
            std::string ResolveLiteral(const LiteralValue& value);
    };
}
