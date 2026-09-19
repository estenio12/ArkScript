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
#include "Global.hpp"
#include "Output.hpp"

namespace ArkScript::Args
{
    void PrintVersion();
    void PrintHelp();

    void BuildArgs(int argc, char** args)
    {
        if(argc < 2) return;
        
        for(int i = 1; i < argc; i++)
        {
            std::string_view cmd = args[i];

            if(cmd == "--f")
            {
                if(i + 1 >= argc) ArkScript::Output::ThrowFatalError("CompilerArguments", "The source file path not passed");
                ArkScript::Global::SOURCE_FILE = args[++i];
            }
            else if(cmd == "--o" || cmd == "-o" || cmd == "--out" || cmd == "--output")
            {
                if(i + 1 >= argc) ArkScript::Output::ThrowFatalError("CompilerArguments", "The output file path not passed");
                ArkScript::Global::OUTPUT_COMPILATION_FILE = args[++i];
            }
            else if(cmd == "--plex")
            {
                if(i + 1 >= argc) ArkScript::Output::ThrowFatalError("CompilerArguments", "The plex output file path not passed");
                ArkScript::Global::PRINT_LEXER_OUTPUT = ArkScript::Global::OUTPUT_FLAG::FILE;
                ArkScript::Global::PRINT_LEXER_OUTPUT_FILE = args[++i];
            }
            else if(cmd == "--v" || cmd == "--version" || cmd == "-v")
            {
                PrintVersion();
                exit(0);
            }
            else if(cmd == "--h" || cmd == "--help" || cmd == "-h")
            {
                PrintHelp();
            }
            else if(cmd == "--no-timer")
            {
                ArkScript::Global::PRINT_TIMER = false;
            }
            else 
            {
                if (cmd.length() > 0 && cmd[0] != '-') 
                {
                    if (ArkScript::Global::SOURCE_FILE.empty()) 
                        ArkScript::Global::SOURCE_FILE = std::string(cmd);
                    else 
                        ArkScript::Output::ThrowFatalError("CompilerArguments", "Multiple source files detected or invalid argument: " + std::string(cmd));
                }
                else 
                {
                    ArkScript::Output::ThrowFatalError("CompilerArguments", "Unknown flag: " + std::string(cmd));
                }
            }
        }
    }

    void PrintVersion()
    {
        ArkScript::Output::PrintInfo("ID: ", false);
        ArkScript::Output::Print(ArkScript::Global::BUILD_ID);
        ArkScript::Output::PrintInfo("Version: ", false);
        ArkScript::Output::Print(ArkScript::Global::BUILD_VERSION);
        ArkScript::Output::PrintInfo("Date: ", false);
        ArkScript::Output::Print(ArkScript::Global::BUILD_DATE);
        ArkScript::Output::PrintInfo("License: ", false);
        ArkScript::Output::Print(ArkScript::Global::BUILD_LICENSE);
    }

    void PrintHelp()
    {
        PrintVersion();
        ArkScript::Output::Print("\nUsage: Arkc [file] [options]\n");
        ArkScript::Output::Print("Options:");
        ArkScript::Output::Print("  --f <path>      Specify source file");
        ArkScript::Output::Print("  --o <path>      Specify output binary path");
        ArkScript::Output::Print("  --plex <path>   Print lexer result into output file");
        ArkScript::Output::Print("  --v, --version  Show version information");
        ArkScript::Output::Print("  --h, --help     Show this help message");
        ArkScript::Output::Print("  --no-timer      Disable compilation timer");
    }
}