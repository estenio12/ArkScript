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
#include <memory>

namespace ArkScript::Ast
{
    struct Node;
    struct ExpressionNode;
    struct StatementNode;
    struct LiteralNode;
    struct IdentifierNode;
    struct BinaryExprNode;
    struct AssignStmtNode;
    struct CallStmtNode;
    struct VarDeclNode;
    struct ConstDeclNode;
    struct ParamNode;
    struct FunDeclNode;
    struct FunCallNode;
    struct ReturnStmtNode;
    struct ModuleDeclNode;
    struct ProgramNode;
    struct ModuleStmtNode;
    struct BlockScopeNode;
    struct ModuleReadonlyDeclNode;
    struct ArgumentList;

    enum class NodeType : uint8_t
    {
        PROGRAM,
        MODULE_DECL,
        MODULE_STMT,
        VAR_DECL,
        FUN_DECL,
        ASSIGN_STMT,
        CALL_STMT,
        RETURN_STMT,
        BINARY_EXPR,
        FUN_CALL,
        ARGUMENTO_LIST,
        IDENTIFIER,
        LITERAL_INT,
        LITERAL_FLOAT,
        LITERAL_CHAR,
        LITERAL_STRING,
        LITERAL_BOOL,
        BLOCK_SCOPE,
        MODULE_READONLY_DECL,
        UNDEFINED
    };

    struct Node
    {
        NodeType type;
        std::string file_path;
        uint32_t line{0};
        uint32_t col{0};
        uint32_t length{0};

        explicit Node(NodeType t) : type(t) {}
        Node(NodeType type, const ArkScript::Token& token) 
        : type(type), line(token.line), col(token.col), length(token.content.length()) {}
        virtual ~Node() = default;

        public:
            void SetLocation(const ArkScript::Token& token)
            {
                this->line = token.line;
                this->col = token.col;
                this->length = token.length();
            }
    };

    struct ModuleMemberNode : public Node
    {
        explicit ModuleMemberNode(NodeType t) : Node(t) {}
        explicit ModuleMemberNode(NodeType t, const ArkScript::Token& token) : Node(t, token) {}
    };

    struct ExpressionNode : public Node
    {
        explicit ExpressionNode(NodeType t) : Node(t) {}
        explicit ExpressionNode(NodeType t, const ArkScript::Token& token) : Node(t, token) {}
    };

    struct StatementNode : public Node
    {
        explicit StatementNode(NodeType t) : Node(t) {}
        explicit StatementNode(NodeType t, const ArkScript::Token& token) : Node(t, token) {}
    };

    struct LiteralNode : public ExpressionNode
    {
        std::string value;
        explicit LiteralNode(NodeType t, std::string val) 
            : ExpressionNode(t), value(std::move(val)) {}
        explicit LiteralNode(const ArkScript::Token& token) 
            : ExpressionNode(NodeType::UNDEFINED, token), value(token.content) {}
    };

    struct IdentifierNode : public ExpressionNode
    {
        std::string name;
        explicit IdentifierNode(std::string id_name) 
            : ExpressionNode(NodeType::IDENTIFIER), name(std::move(id_name)) {}
        explicit IdentifierNode(const ArkScript::Token& token) 
        : ExpressionNode(NodeType::ARGUMENTO_LIST) { this->SetLocation(token); }
    };

    struct BinaryExprNode : public ExpressionNode
    {
        std::string op;
        std::unique_ptr<ExpressionNode> left;
        std::unique_ptr<ExpressionNode> right;
        BinaryExprNode() : ExpressionNode(NodeType::BINARY_EXPR) {}
        explicit BinaryExprNode(const ArkScript::Token& token) 
            : ExpressionNode(NodeType::BINARY_EXPR, token) {}
    };

    struct FunCallNode : public ExpressionNode
    {
        std::string name;
        std::unique_ptr<ArgumentList> argument;
        FunCallNode() : ExpressionNode(NodeType::FUN_CALL) {}
        explicit FunCallNode(const ArkScript::Token& token) 
            : ExpressionNode(NodeType::FUN_CALL, token) {}
    };

    struct ArgumentList : public ExpressionNode
    {
        std::vector<std::unique_ptr<ExpressionNode>> arguments;
        ArgumentList() : ExpressionNode(NodeType::ARGUMENTO_LIST) {}
        explicit ArgumentList(const ArkScript::Token& token) 
            : ExpressionNode(NodeType::ARGUMENTO_LIST, token) {}
    };

    struct VarDeclNode : public StatementNode
    {
        bool is_constant{false};
        std::string name;
        std::string native_type;
        std::unique_ptr<ExpressionNode> initializer;
        VarDeclNode() : StatementNode(NodeType::VAR_DECL){}
        explicit VarDeclNode(const ArkScript::Token& token) 
            : StatementNode(NodeType::VAR_DECL, token) {}
    };

    struct AssignStmtNode : public StatementNode
    {
        std::string name;
        std::unique_ptr<ExpressionNode> expression;
        AssignStmtNode() : StatementNode(NodeType::ASSIGN_STMT) {}
        explicit AssignStmtNode(const ArkScript::Token& token) 
            : StatementNode(NodeType::ASSIGN_STMT, token) {}
    };

    struct CallStmtNode : public StatementNode
    {
        std::unique_ptr<FunCallNode> call;
        CallStmtNode() : StatementNode(NodeType::CALL_STMT) {}
        explicit CallStmtNode(const ArkScript::Token& token) 
            : StatementNode(NodeType::CALL_STMT, token) {}
    };

    struct ReturnStmtNode : public StatementNode
    {
        std::unique_ptr<ExpressionNode> expression;
        ReturnStmtNode() : StatementNode(NodeType::RETURN_STMT) {}
        explicit ReturnStmtNode(const ArkScript::Token& token) 
            : StatementNode(NodeType::RETURN_STMT, token) {}
    };

    struct ParamNode
    {
        std::string name;
        std::string type;

        explicit ParamNode(std::string name, std::string type): name(name), type(type) {};
    };

    struct FunDeclNode : public ModuleMemberNode
    {
        bool is_public{false};
        std::string name;
        std::string return_type;
        std::unique_ptr<BlockScopeNode> body;
        std::vector<std::unique_ptr<ParamNode>> parameters;
        FunDeclNode() : ModuleMemberNode(NodeType::FUN_DECL) {}
        explicit FunDeclNode(const ArkScript::Token& token) 
            : ModuleMemberNode(NodeType::FUN_DECL, token) {}
    };

    struct BlockScopeNode : public Node
    {
        std::vector<std::unique_ptr<StatementNode>> stmts;
        BlockScopeNode() : Node(NodeType::BLOCK_SCOPE) {}

        explicit BlockScopeNode(const ArkScript::Token& token) 
            : Node(NodeType::BLOCK_SCOPE, token) {}
    };

    struct ModuleReadonlyDeclNode : public ModuleMemberNode
    {
        bool is_public{false};
        std::string name;
        std::string native_type;
        std::unique_ptr<ExpressionNode> initializer;
        ModuleReadonlyDeclNode() : ModuleMemberNode(NodeType::MODULE_READONLY_DECL){}
        explicit ModuleReadonlyDeclNode(const ArkScript::Token& token) 
            : ModuleMemberNode(NodeType::MODULE_READONLY_DECL, token) {}
    };

    struct ModuleStmtNode : public Node
    {
        std::vector<std::unique_ptr<ModuleMemberNode>> stmts;
        ModuleStmtNode() : Node(NodeType::MODULE_STMT) {}
        explicit ModuleStmtNode(const ArkScript::Token& token) 
            : Node(NodeType::MODULE_STMT, token) {}
    };

    struct ModuleDeclNode : public Node
    {
        std::string name;
        std::unique_ptr<ModuleStmtNode> stmt;
        ModuleDeclNode() : Node(NodeType::MODULE_DECL) {}
        explicit ModuleDeclNode(const ArkScript::Token& token) 
            : Node(NodeType::MODULE_STMT, token) {}
    };

    struct ProgramNode : public Node
    {
        std::unique_ptr<ModuleDeclNode> module;
        ProgramNode() : Node(NodeType::PROGRAM) {}
    };
}