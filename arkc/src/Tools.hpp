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
#include <algorithm>

namespace Ark::Tools
{
    namespace Str
    {
        inline std::vector<std::string> Split(const std::string& source, char separator)
        {
            if(source.empty()) return std::vector<std::string>();
            std::string buffer;
            buffer.reserve(255);
            std::vector<std::string> result;

            for(const auto& letter : source)
            {
                if(letter == separator)
                {
                    result.push_back(buffer);
                    buffer.clear();
                    continue;
                }
                else
                {
                    buffer.push_back(letter);
                }
            }

            if(!buffer.empty())
            {
                result.push_back(buffer);
                buffer.clear();
            }

            return result;
        }

        inline std::string Trim(const std::string& s)
        {
            const std::string whitespace = " \n\r\t\f\v";
            size_t start = s.find_first_not_of(whitespace);
            if (start == std::string::npos) return "";

            size_t end = s.find_last_not_of(whitespace);
            return s.substr(start, end - start + 1);
        }
    }

    namespace Hash
    {
        // # Hash algorithm FNV-1a. 
        inline uint64_t GenerateHash(const std::string& data) 
        {
            uint64_t hash = 0xcbf29ce484222325ULL;
            const uint64_t prime = 0x100000001b3ULL;

            for (unsigned char c : data) 
            {
                hash ^= static_cast<uint64_t>(c);
                hash *= prime;
            }

            return hash;
        }
    }

    namespace Number
    {
        inline bool IsDigit(char c) { return c >= '0' && c <= '9'; }
    }
}