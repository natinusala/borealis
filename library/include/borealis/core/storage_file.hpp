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
#include <borealis/core/util.hpp>
#include <borealis/core/i18n.hpp>

using namespace brls::literals;
using namespace tinyxml2;

namespace brls 
{

XMLElement *getElementByName(XMLDocument &ele, std::string const &elemt_value)
{
    // Code taken from: https://stackoverflow.com/questions/11921463/find-a-specific-node-in-a-xml-document-with-tinyxml/60964820#60964820
    XMLElement *elem = ele.FirstChildElement(); //Tree root

    while (elem)
    {
        if (!std::string(elem->Value()).compare(elemt_value))
            return elem;
        if (elem->FirstChildElement())
        {
            elem = elem->FirstChildElement();
        }
        else if (elem->NextSiblingElement())
        {
            elem = elem->NextSiblingElement();
        }
        else
        {
            if (elem->Parent()->ToElement()->NextSiblingElement())
            {
                elem = elem->Parent()->ToElement()->NextSiblingElement();
            }
            else if (elem->Parent()->ToElement()->FirstChildElement()
                        &&  strcmp(elem->Name(), elem->Parent()->ToElement()->FirstChildElement()->Name()))
            {
                elem = elem->Parent()->ToElement()->FirstChildElement();
            }
            else {
                break;
            }
        }
    }
    return NULL;
}

#define BRLS_STORAGE_FILE_INIT(filename) StorageFile::init(filename)

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
    file.open(filename, std::ios::out|std::ios::app);
    file.close();

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
    if (!std::filesystem::exists(config_folder + filename)) {
        fatal("File {} not found in {}. ", filename, config_folder);
        return false;
    }

    XMLDocument file;
    file.LoadFile(filename.c_str());

    

    return true;
}

/*
 * Reads a value from the config file, then returns a variable pointing to that value.
 * 
 * For example, if you store a variable with the data "EmreTech is awesome" into a storage file,
 * This function will find that value and return it, so you can read/change the value throughout
 * the program running
 */
T readToFile(std::string name) {
    if (!std::filesystem::exists(config_folder + filename)) {
        fatal("File {} not found in {}. ", filename, config_folder);
        return false;
    }

    XMLDocument file;
    file.LoadFile(filename.c_str());

    auto elementToFind = getElementByName(file, "brls:Property");

}

private:

#ifdef __SWITCH__
std::string config_folder = std::string("/config/") + "brls/appname"_i18n;
#else
std::string config_folder = std::string("./config") + "brls/appname"_i18n;
#endif

std::string filename;

};

}