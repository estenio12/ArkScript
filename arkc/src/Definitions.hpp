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
#include <unordered_set>

namespace ArkScript
{
    inline const std::unordered_set<std::string> KEYWORDS = {
        "i8", "i16", "i32", "i64", 
        "ui8", "ui16", "ui32", "ui64",
        "f32", "f64", "char", "bool",
        "void", "str", "var", "const",
        "pub", "fun", "end", "return", 
        "module"
    };

    namespace LITERAL_VALUES
    {
        const std::string TRUE = "true";
        const std::string FALSE = "false";
    }

    namespace OP_ARITHMETIC
    {
        const std::string ADD = "+";
        const std::string SUB = "-";
        const std::string MUL = "*";
        const std::string DIV = "/";
        const std::string MOD = "%";
    }

    namespace OP_LOGICAL
    {
        const std::string OR  = "||";
        const std::string AND = "&&";
        const std::string NOT = "!";
    }

    namespace OP_COMPARISON
    {
        const std::string EQ   = "==";
        const std::string DIFF = "!=";
        const std::string GT   = ">";
        const std::string LT   = "<";
        const std::string GTEQ = ">=";
        const std::string LTEQ = "<=";
    }

    namespace OP_ASSIGNMENT
    {
        const std::string ASSIGN     = "=";
        const std::string ADDASSIGN  = "+=";
        const std::string SUBASSIGN  = "-=";
        const std::string MULTASSIGN = "*=";
        const std::string DIVASSIGN  = "/=";
        const std::string MODASSIGN  = "%=";
    }

    namespace OP_BITWISE
    {
        const std::string OR     = "|";
        const std::string AND    = "&";
        const std::string XOR    = "^";
        const std::string NOT    = "~";
        const std::string LSHIFT = "<<";
        const std::string RSHIFT = ">>";
    }

    namespace DELIMITER
    {
        const std::string COMMA     = ",";
        const std::string COLON     = ":";
        const std::string LPARAN    = "(";
        const std::string RPARAN    = ")";
        const std::string LBRACE    = "{";
        const std::string RBRACE    = "}";
        const std::string LBRACKET  = "[";
        const std::string RBRACKET  = "]";
        const std::string SEMICOLON = ";";
        const std::string ARROW     = "->";
        const std::string QUOTE     = "'";
        const std::string SCOPEACCESS = "::";
        const std::string DOUBLEQUOTE = "\"";
    }
}