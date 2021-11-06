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

#include <borealis/core/storage_file.hpp>
#include <borealis/core/application.hpp>
#include <borealis/core/assets.hpp>

#include <filesystem>
#include <fstream>

namespace brls
{

std::string IntStorageObject::getTypeName()
{
    return "Int";
}

std::string FloatStorageObject::getTypeName()
{
    return "Float";
}

std::string BoolStorageObject::getTypeName()
{
    return "Bool";
}

std::string StringStorageObject::getTypeName()
{
    return "String";
}

std::string IntListStorageObject::getTypeName()
{
    return "Int";
}

std::string FloatListStorageObject::getTypeName()
{
    return "Float";
}

std::string BoolListStorageObject::getTypeName()
{
    return "Bool";
}

std::string StringListStorageObject::getTypeName()
{
    return "String";
}

bool StorageFile::init(std::string filename, std::string appname)
{
    if (inited)
        return true;

    std::string folder;
    if (brls::Application::getPlatform()->getName() == "GLFW")
        folder = "./config/" + appname + "/";
    else
        folder = "/config/" + appname + "/";

    if (!std::filesystem::exists(folder))
        std::filesystem::create_directories(folder);

    this->filename = filename + ".xml";
    config_path = folder + this->filename;

    if (std::filesystem::exists(config_path))
    {
        inited = true;
        return true;
    }

    std::ofstream file;
    file.open(config_path, std::ios::out|std::ios::app);
    if (file.good())
    {
        file.close();
        inited = true;

        Logger::debug("Successfully made file at {}!", config_path);
        return true;
    }

    file.close();
    Logger::error("StorageFile: Failed to create StorageFile with the name {}.", filename);
    return false;
}

} // namespace brls