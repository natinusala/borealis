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

#include <borealis/core/util.hpp>

namespace brls
{

[[ noreturn ]] void fatal(std::string message)
{
    brls::Logger::error("Fatal error: {}", message);
    throw std::logic_error(message);
}

template <typename T>
char* ConversionUtils::toCString(const T &t) {
    std::ostringstream oss;
    oss << t;

    char *output = new char[sizeof(oss.str().c_str()) + 1];
    std::strcpy(output, oss.str().c_str());
    return output;
}

template <typename T>
T ConversionUtils::fromCString(const char *&s) {
    std::istringstream iss( (std::string(s)) );
    T t;
    iss >> t;
    return t;
}

} // namespace brls
