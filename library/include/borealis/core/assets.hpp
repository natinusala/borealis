/*
    Copyright 2019-2021 natinusala
    Copyright 2019 p-sam
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

#include <filesystem>
#include <string>

#ifndef BRLS_RESOURCES
#error BRLS_RESOURCES define missing
#endif

namespace brls
{
inline static std::string BRLS_ASSET(std::string _str)
{
    _str = std::string(BRLS_RESOURCES) + _str;
    std::filesystem::path tmp_out_path { _str };
    tmp_out_path.make_preferred();
    return tmp_out_path.string();
}
} // namespace brls
