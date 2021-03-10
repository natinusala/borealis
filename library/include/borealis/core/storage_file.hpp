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
#include <vector>
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

// This is a StorageObject, which is a data type of a value stored onto the XML file.
// They have two variables they hold onto throughout their lifetime: the value and name.
// They also have an std::string of their type, since the value will be stored as a char *
// In the XML File
template <typename T>
class StorageObject 
{

public:

StorageObject(T val, std::string type) {this->set(val); this->setType(type)}

void setValue(T newValue) {
    valueSet = newValue;
}

T value() {
    return valueSet;
}

void setName(std::string val) {
    nameInXML = val;
}

std::string name() {
    return nameInXML;
}

void setType(std::string newType) {
    typeOfValue = newType;
} 

std::string type() {
    return typeOfValue;
}

private:
T valueSet;
std::string nameInXML;
std::string typeOfValue;

};

// The StorageFile class allows you to create, write, or read from files for anything
// For example, you can use it for a config or track when a user last used the app
template <typename T>
class StorageFile
{

std::vector<StorageObject<T>> allStorageObjects; // Main std::vector where all StorageObjects are stored.

public:

/*
 * Inits the config folder if it doesn't exist and creates a file.
 * Remember to not include the .xml part in the filename argument, it's already added.
 * If you don't, tinyxml2 will look for file (.)/config/(appname here)/(filename here).xml.xml later on.
 */
bool init(std::string filename) {
    if (!std::filesystem::exists(config_folder))
        std::filesystem::create_directory(config_folder);

    this->filename = filename + ".xml";

    
    std::string config_path = config_folder + this->filename;

    if (std::filesystem::exists(config_path))
        Logger::debug("File exists already. Getting data from the XML file...");

    std::fstream file;
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
 *      <brls:Property name="wizard_shown" value="true" type="boolean"/>
 *	
 *	    <brls:Property name="username" value="h4ck3rm4n" type="string"/>
 *	
 *	    <brls:ListProperty name="favorites">
 *		    <brls:Value value="borealis" />
 *	    </brls:ListProperty>
 * </brls:StorageFile>
 * 
 * It gives a fatal error if the filename is not found (call the init function before these functions).
 */
bool writeToFile(StorageObject<t> value)
{   
    std::string config_path = config_folder + filename;
    T objectFromValue = value.value();
    std::string type = value.type();
    std::string name = value.name()

    FILE *file = fopen(config_path.c_str(), "wb");
    XMLError errorCode;

    XMLDocument doc;
    errorCode = doc.LoadFile(file);

    if (errorCode == 0) // If doc.LoadFile succeded 
    {

        XMLElement *root = doc.RootElement();

        if (!root) // If root is null (which means a new file)
        {
            root = doc.NewElement("brls:StorageFile");
            root->SetText("\n");

            XMLElement *element = doc.NewElement("brls:Property");

            element->SetAttribute("name", name.c_str());
            //element->SetAttribute("value", "str"); TODO: Convert value to char *, then add to attribute
            element->SetAttribute("type", type.c_str());
            root->InsertFirstChild(element);

        } else {
            XMLElement *element = doc.NewElement("brls:Property");

            element->SetAttribute("name", name.c_str());
            //element->SetAttribute("value", "str"); TODO: Convert value to char *, then add to attribute
            element->SetAttribute("type", type.c_str());
            root->InsertFirstChild(element);
        }
    } else {
        Logger::error("TinyXML2 could not open the file. Error code {}.", std::to_string(errorCode));
    }
}

/*
 * Reads a value from the config file, then returns a variable pointing to that value.
 * 
 * For example, if you store a variable with the data "EmreTech is awesome" into a storage file,
 * This function will find that value and return it, so you can read/change the value throughout
 * the program running.
 */
StorageObject<T>& readFromFile(std::string name)
{
    // TODO: Insert function that can parse the XML elements into elements for the std::vector

    size_t numberElement{ 0 };

    for (size_t i{ 0 }; i < allStorageObjects.size(); i++) {
        auto currentElement = allStorageObjects[i];

        if (currentElement.name() == name) {
            numberElement = i;
            break;
        }
    }

    return allStorageObjects[numberElement];
}
private:

#ifdef __SWITCH__ // If the client is running on a Switch, this approach is used
std::string config_folder = std::string("/config/") + "brls/appname"_i18n + "/";
#else // Otherwise, we assume that the client is running on a PC.
std::string config_folder = std::string("./config/") + "brls/appname"_i18n + "/";
#endif
std::string filename;

};

} // namespace brls