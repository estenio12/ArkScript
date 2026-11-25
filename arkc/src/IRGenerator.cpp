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
#include <stdexcept>
#include "IRGenerator.hpp"

namespace ArkScript
{
    static TypeStack MapTypeToStack(const std::string& type_str) 
    {
        if (type_str == "int" || type_str == "bool") return TypeStack::Int;
        if (type_str == "float") return TypeStack::Float;
        if (type_str == "string") return TypeStack::String;
        throw std::runtime_error("IR error: Unknown type for stack mapping: " + type_str);
    }

    std::unique_ptr<Module> IRGenerator::Generate()
    {
        if (!ast || !ast->module) return std::move(current_module);

        current_module->name = ast->module->name;

        if (ast->module->stmt) 
        {
            for (const auto& member : ast->module->stmt->stmts) 
            {
                if (member->type == Ast::NodeType::FUN_DECL) 
                {
                    auto* func_node = static_cast<Ast::FunDeclNode*>(member.get());
                    VisitFunction(*func_node);
                }
                else if (member->type == Ast::NodeType::READONLY_DECL)
                {
                    auto* rdata_node = static_cast<Ast::ReadonlyDeclNode*>(member.get());
                    VisitRData(*rdata_node);
                }
            }
        }

        return std::move(current_module);
    }

    void IRGenerator::VisitFunction(const Ast::FunDeclNode& node)
    {
        ArkScript::Function ir_func;
        ir_func.name = node.name;
        ir_func.return_type = node.return_type;
        ir_func.visibility = node.is_public ? "public" : "private";

        for (const auto& param : node.parameters) 
        {
            ir_func.params.push_back(Param{param->name, param->type});
            ir_func.param_loads.push_back(ParamLoad{MapTypeToStack(param->type), param->name});
        }

        if (node.body) 
        {
            for (const auto& stmt : node.body->stmts) 
            {
                VisitStatement(*stmt, ir_func.body);
            }
        }

        current_module->functions.push_back(std::move(ir_func));
    }

    void IRGenerator::VisitRData(const Ast::ReadonlyDeclNode& node)
    {
        RData ir_rdata;
        ir_rdata.name = node.name;
        ir_rdata.type = node.type;
        ir_rdata.visibility = node.is_public ? "public" : "private";

        if (node.initializer) 
        {
            VisitExpression(*node.initializer, ir_rdata.body);
        }

        TypeStack target_stack = MapTypeToStack(node.type);
        ir_rdata.store_op = Store{target_stack, node.name};

        current_module->rdata_decls.push_back(std::move(ir_rdata));
    }

    void IRGenerator::VisitStatement(const Ast::StatementNode& node, std::vector<Instruction>& field)
    {
        switch (node.type) 
        {
            case Ast::NodeType::VAR_DECL: 
            {
                auto& var_node = static_cast<const Ast::VarDeclNode&>(node);
                if (var_node.initializer) 
                {
                    VisitExpression(*var_node.initializer, field);
                }
                if (var_node.is_constant) 
                {
                    field.push_back(DclConst{var_node.name, var_node.type});
                } 
                else 
                {
                    field.push_back(DclVar{var_node.name, var_node.type});
                }

                field.push_back(Store{MapTypeToStack(var_node.type), var_node.name});
                break;
            }
            case Ast::NodeType::ASSIGN_STMT: 
            {
                auto& assign_node = static_cast<const Ast::AssignStmtNode&>(node);
                if (assign_node.expression) 
                {
                    VisitExpression(*assign_node.expression, field);
                }

                field.push_back(Assign{assign_node.name});
                field.push_back(Store{MapTypeToStack(assign_node.expression->InferredType), assign_node.name});
                break;
            }
            case Ast::NodeType::RETURN_STMT: 
            {
                auto& ret_node = static_cast<const Ast::ReturnStmtNode&>(node);
                
                if (ret_node.expression) 
                {
                    VisitExpression(*ret_node.expression, field);
                }

                field.push_back(Ret{});
                break;
            }
            case Ast::NodeType::CALL_STMT: {
                auto& call_stmt = static_cast<const Ast::CallStmtNode&>(node);
                if (call_stmt.call) {
                    VisitExpression(*call_stmt.call, field);
                }
                break;
            }
            default:
                throw std::runtime_error("IR Error: Statement not implemented in generator.");
        }
    }

    void IRGenerator::VisitExpression(const Ast::ExpressionNode& node, std::vector<Instruction>& field)
    {
        switch (node.type) 
        {
            case Ast::NodeType::LITERAL_INT:
            case Ast::NodeType::LITERAL_FLOAT:
            case Ast::NodeType::LITERAL_CHAR:
            case Ast::NodeType::LITERAL_STRING:
                VisitLiteral(static_cast<const Ast::LiteralNode&>(node), field);
            break;

            case Ast::NodeType::IDENTIFIER: 
            {
                auto& id_node = static_cast<const Ast::IdentifierNode&>(node);
                TypeStack current_stack = MapTypeToStack(node.InferredType);
                field.push_back(Load{current_stack, id_node.name});
                break;
            }
            case Ast::NodeType::BIN_OP:
                VisitBinaryExpr(static_cast<const Ast::BinaryOpNode&>(node), field);
            break;

            case Ast::NodeType::FUN_CALL: 
            {
                auto& call_node = static_cast<const Ast::FunCallNode&>(node);
                int arg_count = 0;
                
                if (call_node.arg_list) 
                {
                    arg_count = static_cast<int>(call_node.arg_list->arguments.size());
                    
                    for (const auto& arg : call_node.arg_list->arguments) 
                    {
                        if (arg) VisitExpression(*arg, field);
                    }
                }

                field.push_back(CallFunc{call_node.name, arg_count});
                break;
            }
            default:
                throw std::runtime_error("IR Error: Invalid expression .");
        }
    }

    void IRGenerator::VisitBinaryExpr(const Ast::BinaryOpNode& node, std::vector<Instruction>& field)
    {
        if (node.left)  VisitExpression(*node.left, field);
        if (node.right) VisitExpression(*node.right, field);

        TypeStack current_stack = MapTypeToStack(node.InferredType);

        if (node.op == "+")       field.push_back(Add{current_stack, 2});
        else if (node.op == "-")  field.push_back(Sub{current_stack, 2});
        else if (node.op == "*")  field.push_back(Mul{current_stack, 2});
        else if (node.op == "/")  field.push_back(Div{current_stack, 2});
        else throw std::runtime_error("IR Error: unsupported binOp: " + node.op);
    }

    void IRGenerator::VisitLiteral(const Ast::LiteralNode& node, std::vector<Instruction>& field)
    {
        if (node.type == Ast::NodeType::LITERAL_INT) 
        {
            int64_t val = std::stoll(node.value);
            field.push_back(Push{TypeStack::Int, val});
        }
        else if (node.type == Ast::NodeType::LITERAL_FLOAT) 
        {
            double val = std::stod(node.value);
            field.push_back(Push{TypeStack::Float, val});
        }
        else if (node.type == Ast::NodeType::LITERAL_STRING) 
        {
            field.push_back(Push{TypeStack::String, node.value});
        }
        else if (node.type == Ast::NodeType::LITERAL_CHAR) 
        {
            char val = node.value.empty() ? '\0' : node.value[0];
            field.push_back(Push{TypeStack::Int, static_cast<int64_t>(val)});
        }
    }
}
