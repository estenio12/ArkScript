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
#include <vector>
#include <variant>
#include <optional>
#include <cstdint>

namespace ArkScript
{
    enum class TypeStack : char {
        Float  = 'f',
        Int    = 'i',
        String = 's'
    };

    using LiteralValue = std::variant<double, int64_t, std::string, char>;

    struct Push { TypeStack stack; LiteralValue value; };
    struct Conv { TypeStack src; TypeStack dest; };
    struct Pop  { TypeStack stack; std::optional<int> count; };
    struct Add  { TypeStack stack; int count; };
    struct Sub  { TypeStack stack; int count; };
    struct Div  { TypeStack stack; int count; };
    struct Mul  { TypeStack stack; int count; };
    struct Load { TypeStack stack; std::string name; };
    struct DclVar   { std::string name; std::string type; };
    struct DclConst { std::string name; std::string type; };
    struct Assign   { std::string name; };
    struct CallFunc { std::string name; int count; };
    struct Ret      { };
    struct Store    { TypeStack stack; std::string name; };

    using Instruction = std::variant<
        Push, Conv, Pop, Add, Sub, Div, Mul, Load,
        DclVar, DclConst, Assign, CallFunc, Ret, Store
    >;

    // Function
    struct Param    { std::string name; std::string type; };
    struct ParamLoad{ TypeStack stack; std::string name; };

    struct Function {
        std::string name;
        std::string return_type;
        std::string visibility;
        std::vector<Param> params;
        std::vector<ParamLoad> param_loads;
        std::vector<Instruction> body;
    };

    struct RData {
        std::string name;
        std::string type;
        std::string visibility;
        std::vector<Instruction> body;
        Store store_op;
    };

    // Module
    struct Module {
        std::string name;
        std::vector<RData> rdata_decls;
        std::vector<Function> functions;
    };
}