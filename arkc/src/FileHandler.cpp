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
#include <filesystem>
#include <fstream>
#include "FileHandler.hpp"
#include "Output.hpp"

std::shared_ptr<Ark::FileDescriptor> Ark::FileHandler::GetFileContent(const std::string& path)
{
    if(path.empty())
        Ark::Output::ThrowFatalError("FileHandler", "Source code file path is required");

    auto absolute_path = Ark::FileHandler::PathResolver(path);
    
    std::ifstream in(absolute_path, std::ios::binary | std::ios::ate);

    if(!in.is_open())
        Ark::Output::ThrowFatalError("FileHandler", "File cannot be read: " + absolute_path);

    std::streamsize size = in.tellg();
    
    auto file_descriptor = std::make_shared<Ark::FileDescriptor>();
    file_descriptor->absolute_path = absolute_path;
    file_descriptor->file_size = static_cast<uint64_t>(size);

    if(size > 0) 
    {
        in.seekg(0, std::ios::beg);
        file_descriptor->source_code.resize(size);
        if(!in.read(file_descriptor->source_code.data(), size))
            Ark::Output::ThrowFatalError("FileHandler", "Error reading content from: " + absolute_path);
    }

    in.close();
    return file_descriptor; 
}

std::string Ark::FileHandler::PathResolver(const std::string& path)
{
    try 
    {
        return std::filesystem::absolute(path).string();
    } 
    catch (const std::filesystem::filesystem_error& e) 
    {
        Ark::Output::ThrowFatalError("FileHandler", "Could not resolve path: " + path);
    }
}


