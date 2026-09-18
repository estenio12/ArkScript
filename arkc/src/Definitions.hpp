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
#include <string_view>
#include <unordered_set>

namespace Ark
{
    namespace KEYWORDS
    {
        const std::string TI8        = "i8";
        const std::string TI16       = "i16";
        const std::string TI32       = "i32";
        const std::string TI64       = "i64";
        const std::string TUI8       = "ui8";
        const std::string TUI16      = "ui16";
        const std::string TUI32      = "ui32";
        const std::string TUI64      = "ui64";
        const std::string TF32       = "f32";
        const std::string TF64       = "f64";
        const std::string TCHAR      = "char";
        const std::string TBOOL      = "bool";
        const std::string TVOID      = "void";
        const std::string TSTRING    = "string";
        const std::string TRETURN    = "return";
        const std::string TINTERNAL  = "internal";
        const std::string TVAR       = "var";
        const std::string TCONST     = "const";
        const std::string TEND       = "end";
        const std::string TEXTENDS   = "extends";
        const std::string TFUN       = "fun";
        const std::string TMODULE    = "module";
        const std::string TIMPORT    = "import";
        const std::string TIF        = "if";
        const std::string TELIF      = "elif";
        const std::string TELSE      = "else";
        const std::string TFOR       = "for";
        const std::string TWHILE     = "while";
        const std::string TENUM      = "enum";
        const std::string TIN        = "in";
        const std::string TOF        = "of";
        const std::string TAT        = "at";
        const std::string TEXTERN    = "extern";
        const std::string TTRUE      = "true";
        const std::string TFALSE     = "false";
        const std::string TSTRUCT    = "struct";
        const std::string TCONTINUE  = "continue";
        const std::string TMATCH     = "match";

        inline const std::unordered_set<std::string> keywords = {
            Ark::KEYWORDS::TI8,
            Ark::KEYWORDS::TI16,
            Ark::KEYWORDS::TI32,
            Ark::KEYWORDS::TI64,
            Ark::KEYWORDS::TUI8,
            Ark::KEYWORDS::TUI16,
            Ark::KEYWORDS::TUI32,
            Ark::KEYWORDS::TUI64,
            Ark::KEYWORDS::TF32,
            Ark::KEYWORDS::TF64,
            Ark::KEYWORDS::TCHAR,
            Ark::KEYWORDS::TBOOL,
            Ark::KEYWORDS::TVOID,
            Ark::KEYWORDS::TSTRING,
            Ark::KEYWORDS::TRETURN,
            Ark::KEYWORDS::TINTERNAL,
            Ark::KEYWORDS::TVAR,
            Ark::KEYWORDS::TCONST,
            Ark::KEYWORDS::TEND,
            Ark::KEYWORDS::TEXTENDS,
            Ark::KEYWORDS::TFUN,
            Ark::KEYWORDS::TMODULE,
            Ark::KEYWORDS::TIMPORT,
            Ark::KEYWORDS::TIF,
            Ark::KEYWORDS::TELIF,
            Ark::KEYWORDS::TELSE,
            Ark::KEYWORDS::TFOR,
            Ark::KEYWORDS::TWHILE,
            Ark::KEYWORDS::TENUM,
            Ark::KEYWORDS::TIN,
            Ark::KEYWORDS::TOF,
            Ark::KEYWORDS::TAT,
            Ark::KEYWORDS::TEXTERN,
            Ark::KEYWORDS::TTRUE,
            Ark::KEYWORDS::TFALSE,
            Ark::KEYWORDS::TSTRUCT,
            Ark::KEYWORDS::TCONTINUE,
            Ark::KEYWORDS::TMATCH,
        };
    }

    namespace OP_ARITHMETIC
    {
        const std::string_view ADD = "+";
        const std::string_view SUB = "-";
        const std::string_view MUL = "*";
        const std::string_view DIV = "/";
        const std::string_view MOD = "%";
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

    namespace Ast
    {
        enum class AstType : uint8_t
        {
            LITERAL_INT, LITERAL_FLOAT, LITERAL_BOOL, LITERAL_CHAR, LITERAL_STRING, LITERAL_VALUE,
            TYPE_NATIVE, IDENTIFIER, TYPE_ARRAY, TYPE_LIST, CALL_FUNCTION, ARGUMENT_LIST, CONDITION_STMT, INTERATOR,
            ENUM_ITEM, ENUM_ITEM_LIST, ENUM_BLOCK_SCOPE, ENUM_DECLARATION, FOR_STMT, WHILE_STMT,
            LOOP_DECLARATION, RETURN_STMT, STATEMENT, BLOCK_SCOPE, EXPRESSION, VARIABLE_DECLARATION,
            CONSTANT_DECLARATION, ACCESS_MODIFIER, DATA_DECLARATION, GENERIC_PARAM, GENERIC_ARGS, STRUCT_VALUE_ASSIGNMENT,
            STRUCT_VALUE_ASSIGN_LIST, STRUCT_INSTANCE, STRUCT_BLOCK_SCOPE, STRUCT_INHERIT, STRUCT_DECLARATION, PARAMETER,
            PARAMETER_LIST, TYPE_IDENTIFER, TYPE_FUNCTION, FUNCTION_DECLARATION, ANONIMOUS_FUNCTION,
            EXTERN_DECLARATION, MODULE_DECLARATION, IMPORT_DECLARATION, BIN_OP, UNARY_OP, UNKNOW,
            ASSIGNMENT_STMT, OP_ASSIGNMENT, LOOP_BLOCK_SCOPE, CAST
        };
    }
}