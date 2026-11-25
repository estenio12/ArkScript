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
#include "IROptimizer.hpp"
#include <variant>

namespace ArkScript
{
    void IROptimizer::Optimize(Module& module)
    {
        for (auto& rdata : module.rdata_decls)
        {
            bool changed = true;
            while (changed) 
            {
                bool folded = ApplyConstantFolding(rdata.body);
                bool neutral = ApplyNeutralOperations(rdata.body);
                changed = folded || neutral;
            }
        }

        for (auto& func : module.functions)
        {
            bool changed = true;
            while (changed) 
            {
                bool folded = ApplyConstantFolding(func.body);
                bool neutral = ApplyNeutralOperations(func.body);
                bool dce = ApplyDeadCodeElimination(func.body);
                changed = folded || neutral || dce;
            }
        }
    }

    bool IROptimizer::ApplyConstantFolding(std::vector<Instruction>& body)
    {
        if (body.size() < 3) return false;

        for (size_t i = 0; i < body.size() - 2; ++i)
        {
            if (std::holds_alternative<Push>(body[i]) && std::holds_alternative<Push>(body[i + 1]))
            {
                auto& p1 = std::get<Push>(body[i]);
                auto& p2 = std::get<Push>(body[i + 1]);
                auto& op_inst = body[i + 2];

                if (p1.stack != p2.stack) continue;

                if (p1.stack == TypeStack::Int && std::holds_alternative<int64_t>(p1.value) && std::holds_alternative<int64_t>(p2.value))
                {
                    int64_t v1 = std::get<int64_t>(p1.value);
                    int64_t v2 = std::get<int64_t>(p2.value);
                    int64_t resultado = 0;
                    bool opt_valida = false;

                    if (std::holds_alternative<Add>(op_inst)) { resultado = v1 + v2; opt_valida = true; }
                    else if (std::holds_alternative<Sub>(op_inst)) { resultado = v1 - v2; opt_valida = true; }
                    else if (std::holds_alternative<Mul>(op_inst)) { resultado = v1 * v2; opt_valida = true; }
                    else if (std::holds_alternative<Div>(op_inst) && v2 != 0) { resultado = v1 / v2; opt_valida = true; }

                    if (opt_valida)
                    {
                        body[i] = Push{TypeStack::Int, resultado};
                        body.erase(body.begin() + i + 1, body.begin() + i + 3);
                        return true;
                    }
                }
                else if (p1.stack == TypeStack::Float && std::holds_alternative<double>(p1.value) && std::holds_alternative<double>(p2.value))
                {
                    double v1 = std::get<double>(p1.value);
                    double v2 = std::get<double>(p2.value);
                    double resultado = 0.0;
                    bool opt_valida = false;

                    if (std::holds_alternative<Add>(op_inst)) { resultado = v1 + v2; opt_valida = true; }
                    else if (std::holds_alternative<Sub>(op_inst)) { resultado = v1 - v2; opt_valida = true; }
                    else if (std::holds_alternative<Mul>(op_inst)) { resultado = v1 * v2; opt_valida = true; }
                    else if (std::holds_alternative<Div>(op_inst) && v2 != 0.0) { resultado = v1 / v2; opt_valida = true; }

                    if (opt_valida)
                    {
                        body[i] = Push{TypeStack::Float, resultado};
                        body.erase(body.begin() + i + 1, body.begin() + i + 3);
                        return true;
                    }
                }
            }
        }
        return false;
    }

    bool IROptimizer::ApplyNeutralOperations(std::vector<Instruction>& body)
    {
        if (body.size() < 3) return false;

        for (size_t i = 1; i < body.size() - 1; ++i)
        {
            if (std::holds_alternative<Push>(body[i]))
            {
                auto& neutral_push = std::get<Push>(body[i]);
                auto& op_inst = body[i + 1];

                if (neutral_push.stack == TypeStack::Int && std::holds_alternative<int64_t>(neutral_push.value))
                {
                    int64_t val = std::get<int64_t>(neutral_push.value);
                    bool descartar_operacao = false;

                    if ((std::holds_alternative<Add>(op_inst) || std::holds_alternative<Sub>(op_inst)) && val == 0)
                    {
                        descartar_operacao = true;
                    }
                    else if ((std::holds_alternative<Mul>(op_inst) || std::holds_alternative<Div>(op_inst)) && val == 1)
                    {
                        descartar_operacao = true;
                    }

                    if (descartar_operacao)
                    {
                        body.erase(body.begin() + i, body.begin() + i + 2);
                        return true;
                    }
                }
                else if (neutral_push.stack == TypeStack::Float && std::holds_alternative<double>(neutral_push.value))
                {
                    double val = std::get<double>(neutral_push.value);
                    bool descartar_operacao = false;

                    if ((std::holds_alternative<Add>(op_inst) || std::holds_alternative<Sub>(op_inst)) && val == 0.0)
                    {
                        descartar_operacao = true;
                    }
                    else if ((std::holds_alternative<Mul>(op_inst) || std::holds_alternative<Div>(op_inst)) && val == 1.0)
                    {
                        descartar_operacao = true;
                    }

                    if (descartar_operacao)
                    {
                        body.erase(body.begin() + i, body.begin() + i + 2);
                        return true;
                    }
                }
            }
        }
        
        return false;
    }

    bool IROptimizer::ApplyDeadCodeElimination(std::vector<Instruction>& body)
    {
        for (size_t i = 0; i < body.size(); ++i)
        {
            if (std::holds_alternative<Ret>(body[i]))
            {
                if (i + 1 < body.size())
                {
                    body.erase(body.begin() + i + 1, body.end());
                    return true;
                }
            }
        }
        return false;
    }
}
