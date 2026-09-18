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
#include <memory>

namespace Ark
{
    struct FileDescriptor
    {
        std::string absolute_path;
        std::string source_code;
        uint64_t file_size = 0;
    };

    class FileHandler
    {
        public:
            static std::shared_ptr<FileDescriptor> GetFileContent(const std::string& path);
            
        private:
            static std::string PathResolver(const std::string& path);
    };
}
