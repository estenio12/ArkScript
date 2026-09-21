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
#include <memory>
#include <chrono>
#include "Args.hpp"
#include "Global.hpp"
#include "Output.hpp"
#include "Compiler.hpp"

int main(int argc, char** argv)
{
    auto timer_start = std::chrono::high_resolution_clock::now();
    ArkScript::Output::WindowsInitializeConfig();
    ArkScript::Args::BuildArgs(argc, argv);

    if(ArkScript::Global::SOURCE_FILE.empty())
    {
        ArkScript::Output::PrintError("No source file provided. Use --h for help.");
        return EXIT_FAILURE; 
    }

    try 
    {
        ArkScript::Compiler(ArkScript::Global::SOURCE_FILE);

        auto timer_end = std::chrono::high_resolution_clock::now();
        auto elapsed_time = std::chrono::duration_cast<std::chrono::milliseconds>(timer_end - timer_start);
        ArkScript::Output::Print("\nCompilation completed in: " + std::to_string(elapsed_time.count()) + "ms\n");
    }
    catch (const std::exception& e) 
    {
        ArkScript::Output::ThrowFatalError("Core", std::string("Unexpected error: ") + e.what());
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}