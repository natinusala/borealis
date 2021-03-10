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
#include <stdio.h>
#include <string.h>
#include <string>
#include <type_traits>
#include <borealis/core/logger.hpp>
#include <borealis/core/util.hpp>
#include <borealis/core/i18n.hpp>

using namespace brls::literals;
using namespace tinyxml2;

namespace brls 
{

#define BRLS_STORAGE_FILE_INIT(filename) bool initSuccess = this->init(filename)

#define BRLS_STORAGE_FILE_WRITE_DATA(value, name) this->writeToFile(value, name)
#define BRLS_STORAGE_FILE_READ_DATA(value, name) auto value = this->readFromFile(name)

// The StorageFile class allows you to create, write, or read from files for anything
// For example, you can use it for a config or track when a user last used the app
template <typename T>
class StorageFile
{

public:

/*
 * Inits the config folder if it doesn't exist and creates a file.
 */
bool init(std::string filename) {
    if (!std::filesystem::exists(config_folder))
        std::filesystem::create_directory(config_folder);

    this->filename = filename + ".xml";

    std::fstream file;
    std::string config_path = config_folder + this->filename;
    file.open(config_path, std::ios::out|std::ios::app);
    file.close();

    Logger::debug("Successfully made file at {}!", config_path);

    return true;
}

/*
 * Writes a value to the config file.
 * 
 * An example would be the following:
 * 
 * <brls:StorageFile>
 *      <brls:Property name="wizard_shown" value="true" />
 *	
 *	    <brls:Property name="username" value="h4ck3rm4n" />
 *	
 *	    <brls:ListProperty name="favorites">
 *		    <brls:Value value="borealis" />
 *	    </brls:ListProperty>
 * </brls>
 * 
 * It gives a fatal error if the filename is not found (call the init function before these functions)
 */
bool writeToFile(T value, std::string name) {
    std::string config_path = config_folder + filename;
    if (!std::filesystem::exists(config_path)) {
        Logger::error("file {} not found in {}. ", filename, config_folder);
        return false;
    }

    Logger::debug("Passed config path exists test");

    FILE *file = fopen(config_path.c_str(), "wb");

    Logger::debug("Made FILE * variable");

    XMLDocument doc;
    XMLError errorCode;
    errorCode = doc.LoadFile(file);

    if (errorCode != 0) {

        auto root = doc.RootElement();

        if (!root) {
            auto newElement = doc.NewElement("brls:StorageFile");
            doc.InsertFirstChild(newElement);
            doc.SaveFile(file);
            Logger::debug("Empty xml file, added contents");
        }

        root = doc.RootElement();

        auto newElement = doc.NewElement("brls:Property");
        newElement->SetAttribute("name", name.c_str());
        if (std::is_same<T, std::string>::value) { // TODO: Find a different solution to compare a template type with std::string
            //newElement->SetAttribute("value", value.c_str());
            newElement->SetAttribute("value", value.c_str());
            Logger::debug("std::string detected");
        } else {
            //newElement->SetAttribute("value", value);
        }

        if (!root) Logger::error("null pointer to root element");

        root->InsertEndChild(newElement);
        Logger::debug("Inserted element into XML file");

        fclose(file);
        errorCode = doc.SaveFile(file);

        if (errorCode != 0) {
            fclose(file);
            return true;
        } else {
            Logger::error("failed to save file via TinyXML2.");
            return false;
        }

    } else {
        Logger::error("failed to open file via TinyXML2.");
        return false;
    }
}

/*
 * Reads a value from the config file, then returns a variable pointing to that value.
 * 
 * For example, if you store a variable with the data "EmreTech is awesome" into a storage file,
 * This function will find that value and return it, so you can read/change the value throughout
 * the program running
 */
T readFromFile(std::string name) {
    std::string config_path = config_folder + filename;
    if (!std::filesystem::exists(config_path)) {
        Logger::error("file {} not found in {}. ", filename, config_folder);
        return false;
    }

    XMLDocument file;
    XMLError errorCode;
    errorCode = file.LoadFile(filename.c_str());

    if (errorCode == 0) {
        auto elementToFind = file.FirstChildElement("brls:Property");
        const char * nameFromElement;

        // Uses an infinite while loop to find the right element to read from
        while (true) {
            elementToFind->QueryStringAttribute("name", &nameFromElement);

            if (nameFromElement == name.c_str()) 
                break;
            else 
                elementToFind = file.NextSiblingElement("brls:Property");
            
        }
        T value;

        /*elementToFind->QueryAttribute("value", &value);
        TODO: Find a better solution than QueryAttribute*/

        return value;
    } else {
        Logger::error("failed to open file via TinyXML2.");
        return NULL;
    }
}

private:

#ifdef __SWITCH__ // If the client is running on a Switch, this approach is used
std::string config_folder = std::string("/config/") + "brls/appname"_i18n + "/";
#else // Otherwise, we assume that the client is running on a PC.
std::string config_folder = std::string("./config/") + "brls/appname"_i18n + "/";
#endif

std::string filename;

};

}