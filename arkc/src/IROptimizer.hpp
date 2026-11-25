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
#include <memory>
#include <vector>
#include "IR.hpp"

namespace ArkScript
{
    class IROptimizer
    {
        public:
            IROptimizer() = default;

            void Optimize(Module& module);

        private:
            bool ApplyConstantFolding(std::vector<Instruction>& body);
            bool ApplyDeadCodeElimination(std::vector<Instruction>& body);
            bool ApplyNeutralOperations(std::vector<Instruction>& body);
    };
}
