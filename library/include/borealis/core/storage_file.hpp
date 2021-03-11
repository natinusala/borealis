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
#include <sstream>
#include <fstream>
#include <stdio.h>
#include <string.h>
#include <string>
#include <vector>
#include <type_traits>
#include <borealis/core/logger.hpp>
#include <borealis/core/util.hpp>
#include <borealis/core/i18n.hpp>

using namespace brls::literals;
using namespace tinyxml2;

template <typename T1, typename T2>
struct is_same_type {static const bool result = false;};

template <typename T>
struct is_same_type<T,T> {static const bool result = true;};

template <typename R>
R charToOther(const char * input) {
    if (is_same_type<R, std::basic_string<char>>().result == true) {
        return std::string(input);
    } else if (is_same_type<R, char*>().result == true) {
        return input;
    } else {
        std::istringstream iss(input);
        R output;
        iss >> output;
        return output;
    }
}

template <typename R>
char* otherToChar(R input) {
    if (is_same_type<R, std::basic_string<char>>().result == true) {
        char * output;
        std::strcpy(output, input.c_str());
        return output;
    } else if (is_same_type<R, char*>().result == true) {
        char * output;
        std::strcpy(output, input.c_str());
        return output;
    } else {
        std::istringstream iss(input);
        char * output;
        std::strcpy(output, iss.str().c_str());
        return output;
    }
}

namespace brls 
{

#define BRLS_STORAGE_FILE_INIT(filename) bool initSuccess = this->init(filename)
#define BRLS_STORAGE_FILE_WRITE_DATA(StorageObject) this->writeToFile(StorageObject)
#define BRLS_STORAGE_FILE_READ_DATA(StorageObject, name) auto StorageObject = this->readFromFile(name)
#define BRLS_STORAGE_FILE_STORAGE_OBJECT(variableName, value, name, type) auto variableName = this->createStorageObject(value, name, type)
#define BRLS_STORAGE_FILE_BLANK_STORAGE_OBJECT(variableName, type) brls::StorageObject<type> variableName = this->createStorageObject()

// This is a StorageObject, which is a data type of a value stored onto the XML file.
// They have two variables they hold onto throughout their lifetime: the value and name.
// They also have an std::string of their type, since the value will be stored as a char *
// In the XML File
template <typename T>
class StorageObject 
{

public:

StorageObject() {}
StorageObject(T val, std::string name, std::string type) {this->setValue(val); this->setName(name); this->setType(type);}

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
// For example, you can use it for a config or track when a user last used the app.
// The template is required and can only be one type. You can, however, transform the type into something else
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
    
    this->setConfigPath();

    if (std::filesystem::exists(config_path)) {
        Logger::debug("File already exists. Quitting out of init function...");
        return true;
    }

    std::fstream file;
    file.open(config_path, std::ios::out|std::ios::app);
    file.close();

    Logger::debug("Successfully made file at {}!", config_path);

    return true;
}

void setConfigPath() {
    config_path = config_folder + filename;
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
bool writeToFile(StorageObject<T> value)
{   
    // Gets values from the StorageObject and configures the config_path variable
    T objectFromValue = value.value();
    std::string type = value.type();
    std::string name = value.name();

    if (!std::filesystem::exists(config_path))
        Logger::error("File not found. Make sure to call the init function before this.");

    // Opens a file from stdio (tinyxml2 doesn't support STL) and defines an errorCode variable
    //FILE *file = fopen(config_path.c_str(), "wb");
    XMLError errorCode;

    // Defines a doc variable and attempts to load the file
    XMLDocument doc;
    //errorCode = doc.LoadFile(file);
    errorCode = doc.LoadFile(config_path.c_str());

    if (errorCode == 0) // If doc.LoadFile succeded 
    {

        XMLElement *root = doc.RootElement(); // We determine the root element (null or an actual element)

        if (!root) // If root is null (which means a new file)
        {
            // New root element
            XMLNode *pRoot = doc.NewElement("brls:StorageFile");
            doc.InsertFirstChild(pRoot);
            Logger::debug("Made new root element");

            // New child element
            XMLElement *element = doc.NewElement("brls:Property");
            Logger::debug("Made new child element");

            element->SetAttribute("name", name.c_str());
            Logger::debug("Name attribute set");
            if (is_same_type<T, std::string>().result == true)
                element->SetAttribute("value", objectFromValue.c_str());
            else
                element->SetAttribute("value", otherToChar<T>(objectFromValue));
            Logger::debug("Value attribute set");
            element->SetAttribute("type", type.c_str());
            Logger::debug("Type attribute set");
            pRoot->InsertFirstChild(element);
            Logger::debug("Insertted into root");

            // We try to save the file
            //errorCode = doc.SaveFile(file);
            //fclose(file);

            doc.SaveFile(config_path.c_str());
            Logger::debug("Saved file");

            if (errorCode == 0) // If it succedded
                return true;
            else { // Otherwise
                Logger::error("TinyXML2 could not save the file. Error code {}.", std::to_string(errorCode));
                return false;
            }

        } else { // Otherwise if there is a root element
            // New child element
            XMLElement *element = doc.NewElement("brls:Property");
            Logger::debug("Made new child element");

            element->SetAttribute("name", name.c_str());
            Logger::debug("Name attribute set");
            if (is_same_type<T, std::string>().result == true)
                element->SetAttribute("value", objectFromValue.c_str());
            else
                element->SetAttribute("value", otherToChar<T>(objectFromValue));
            Logger::debug("Value attribute set");
            element->SetAttribute("type", type.c_str());
            Logger::debug("Type attribute set");
            root->InsertFirstChild(element);
            Logger::debug("Insertted into root");

            // We try to save the file
            //errorCode = doc.SaveFile(file);
            //fclose(file);

            doc.SaveFile(config_path.c_str());
            Logger::debug("Saved file");

            if (errorCode == 0) // If it succedded
                return true;
            else { // Otherwise
                Logger::error("TinyXML2 could not save the file. Error code {}.", std::to_string(errorCode));
                Logger::error("More details: {}", doc.ErrorStr());
                return false;
            }
        }
    } else { // Otherwise
        Logger::error("TinyXML2 could not open the file. Error code {}.", std::to_string(errorCode));
        Logger::error("More details: {}", doc.ErrorStr());
        return false;
    }
}

void parseXMLToVector(std::string name) 
{
    //FILE *file = fopen(config_path.c_str(), "rb");

    XMLError errorCode;
    XMLDocument doc;

    //errorCode = doc.LoadFile(file);
    errorCode = doc.LoadFile(config_path.c_str());

    if (errorCode == 0)
    {
        XMLNode *root = doc.RootElement();
        Logger::debug("Root node created");

        if (!root) {
            Logger::error("NULL root element. This means the XML file is blank (hasn't been written to yet), or the XML file is broken.");
            return;
        }

        XMLElement *element = root->FirstChildElement();
        Logger::debug("elementToBeFound variable defined");

        for (XMLElement *e = root->FirstChildElement(); e != NULL;
            e = e->NextSiblingElement()) {
            Logger::debug("Looping through all child elements in root element...");
            Logger::debug("{}", e->Attribute("name"));

            if (e->Attribute("name") == name.c_str()) {
                element = e;
                break;
            }
        }

        const char * valueOfValue;
        const char * valueOfType;

        element->QueryStringAttribute("value", &valueOfValue);
        element->QueryStringAttribute("type", &valueOfType);
        Logger::debug("Attributes extracted");

        StorageObject<T> obj{};

        Logger::debug("{} {} {}", valueOfValue, name, valueOfType);

        obj.setName(name);
        obj.setType(std::string(valueOfType));

        char * newValueOfValue = new char[100];
        std::strcpy(newValueOfValue, valueOfValue);
        T actualVal = charToOther<T>(newValueOfValue);
        obj.setValue(actualVal);

        Logger::debug("Set elements of the StorageObject");

        allStorageObjects.push_back(obj);
        Logger::debug("Pushed StorageObject into vector");

    } else {
        Logger::error("TinyXML2 could not open the file. Error code {}.", std::to_string(errorCode));
        Logger::error("More details: {}", doc.ErrorStr());
        return;
    }
}

/*
 * Reads a value from the config file, then returns a variable pointing to that value.
 * 
 * For example, if you store a variable with the data "EmreTech is awesome" into a storage file,
 * This function will find that value and return it, so you can read/change the value throughout
 * the program running.
 */
StorageObject<T> readFromFile(std::string name)
{
    parseXMLToVector(name);

    size_t numberElement{ 0 }; // Defines numberElement for the return

    for (size_t i{ 0 }; i < allStorageObjects.size(); i++) { // For loop to loop through all elements of the std::vector
        auto currentElement = allStorageObjects[i]; // Set current element 

        if (currentElement.name() == name) { // If the current element name is equal to the name we are looking for
            numberElement = i; // Set numberElement to i and break the loop
            break;
        }
    }

    return allStorageObjects[numberElement]; // Return the StorageObject object from the std::vector
}

/*
 * Allows you to check if the file is empty or not.
 * Returns true if it's empty, false if it's not 
 */
bool isFileEmpty() {
    //FILE *file = fopen(config_path.c_str(), "rb");

    XMLDocument doc;
    //doc.LoadFile(file);
    doc.LoadFile(config_path.c_str());

    XMLElement *root = doc.RootElement();
    if (!root) {
        //fclose(file);
        Logger::debug("True returned");
        return true;
    }
    else {
        //fclose(file);
        Logger::debug("False returned");
        return false;
    } 
}

/*
 * Creates a new StorageObject for the child class.
 */
StorageObject<T> createStorageObject(T value, std::string name, std::string type)
{
    return StorageObject<T>(value, name, type); // Returns a new StorageObject
}

/*
 * Creates a new blank StorageObject for the child class.
 */
StorageObject<T> createStorageObject()
{
    return StorageObject<T>(); // Returns a blank StorageObject
}

private:

#ifdef __SWITCH__ // If the client is running on a Switch, this approach is used
std::string config_folder = std::string("/config/") + "brls/appname"_i18n + "/";
#else // Otherwise, we assume that the client is running on a PC.
std::string config_folder = std::string("./config/") + "brls/appname"_i18n + "/";
#endif
std::string filename;

std::string config_path; // This is blank until Init function is called

};

} // namespace brls