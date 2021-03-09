/*
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

#include <tinyxml2/tinyxml2.h>
#include <filesystem>
#include <iostream>
#include <fstream>
#include <string>
#include <borealis/core/logger.hpp>
#include <borealis/core/i18n.hpp>

using namespace brls::literals;

namespace brls 
{

// The StorageFile class allows you to create, write, or read from files for anything
// For example, you can use it for a config or track when a user last used the app
class StorageFile
{

public:

/*
 * Inits the config folder if it doesn't exist and creates a file
 */
bool init(std::string filename) {
    if (!std::filesystem::exists(config_folder))
        std::filesystem::create_directory(config_folder);

    std::fstream file;
    file.open(filename, std::ios::out|std::ios::app);
    file.close();

    return true;
}


private:

#ifdef __SWITCH__
std::string config_folder = std::string("/config/") + "brls/appname"_i18n;
#else
std::string config_folder = std::string("./config") + "brls/appname"_i18n;
#endif

};

}