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

#include <iostream>

namespace ArkScript::Output
{
    const std::string RESET   = "\033[0m";
    const std::string RED     = "\033[31m";
    const std::string GREEN   = "\033[32m";
    const std::string YELLOW  = "\033[33m";
    const std::string BOLD    = "\033[1m";
    const std::string BLUE    = "\033[34m";

    void WindowsInitializeConfig();

    inline void Print(std::string message, bool break_line = true, bool use_bold = false)
    {
        std::cout << (use_bold ? BOLD : "") << message << RESET << (break_line ? "\n" : "");
    }
    
    inline void PrintError(std::string message, bool break_line = true, bool use_bold = false)
    {
        std::cout << (use_bold ? BOLD : "") << RED << message << RESET << (break_line ? "\n" : "");
    }

    inline void PrintSuccess(std::string message, bool break_line = true, bool use_bold = false)
    {
        std::cout << (use_bold ? BOLD : "") << GREEN << message << RESET << (break_line ? "\n" : "");
    }

    inline void PrintWarn(std::string message, bool break_line = true, bool use_bold = false)
    {
        std::cout << (use_bold ? BOLD : "") << YELLOW << message << RESET << (break_line ? "\n" : "");
    }

    inline void PrintInfo(std::string message, bool break_line = true, bool use_bold = false)
    {
        std::cout << (use_bold ? BOLD : "") << BLUE << message << RESET << (break_line ? "\n" : "");
    }

    [[noreturn]] inline void ThrowFatalError(std::string origin, std::string message)
    {
        Output::PrintError(origin + ": " + message, true, true);
        exit(1);
    }
};