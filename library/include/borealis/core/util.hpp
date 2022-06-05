/*
    Copyright 2021 natinusala
    Copyright 2021 EmreTech

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/

#pragma once

#include <borealis/core/logger.hpp>
#include <sstream>
#include <stdexcept>

namespace brls
{

/**
 * Prints the given error message message and throws a std::logic_error.
 */
[[noreturn]] void fatal(std::string message);

struct ConversionUtils
{ 
    /*
     * Converts any primitive type to a C-style string (char*)
     */
    template <typename T>
    inline static char* toCString(const T& t)
    {
        std::ostringstream oss;
        oss << std::boolalpha << t;

        char* output = new char[sizeof(oss.str().c_str()) + 1];
        std::strcpy(output, oss.str().c_str());
        return output;
    }

    /*
     * Converts a C-style string (char*) to any primitive type
     */
    template <typename T>
    inline static T fromCString(const char* s)
    {
        std::istringstream iss((std::string(s)));
        T t;
        iss >> std::boolalpha >> t;
        return t;
    }
};

} // namespace brls
