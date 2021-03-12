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

// The following are misc functions that are used for the rest of this program.
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
    if (is_same_type<R, std::basic_string<char>>().result == true) { // This will somehow be true no matter what (maybe I should use a preprocessor check?)
        char * output;
        std::strcpy(output, input.c_str());
        return output;
    } else if (is_same_type<R, char*>().result == true) {
        char * output;
        std::strcpy(output, input.c_str());
        return output;
    } else {
        std::istringstream ss;
        ss << input;
        char * output;
        std::strcpy(output, iss.str().c_str());
        return output;
    }
}

namespace brls 
{

// Macros (more will be added once the whole Storage system is simpilified)
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

void setIsEmpty(bool newVal) {
    isEmpty = newVal;
}

bool getIsEmpty() {
    return isEmpty;
}

private:
T valueSet;
std::string nameInXML;
std::string typeOfValue;

bool isEmpty;

};

// The StorageFile class allows you to create, write, or read from files for anything
// For example, you can use it for a config or track when a user last used the app.
// The template is required and can only be one type. You can, however, transform the type into something else
template <typename T>
class BasicStorageFile
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
    
    this->setConfigPath(filename);

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

/*
 * Writes a value to the storage file.
 * 
 * An example would be the following: 
 *
 *   <brls:StorageFile>
 *       <brls:Property name="wizard_shown" value="true" type="boolean"/>
 *	
 *	    <brls:Property name="username" value="h4ck3rm4n" type="string"/>
 *	
 *	    <brls:ListProperty name="favorites">
 *		    <brls:Value value="borealis" />
 *	    </brls:ListProperty>
 *   </brls:StorageFile>
 * 
 * It gives a fatal error if the filename is not found (call the init function before these functions).
 */
bool writeToFile(StorageObject<T> value, bool overwriteExisting = true)
{   
    // Gets values from the StorageObject and configures the config_path variable
    T objectFromValue = value.value();
    std::string type = value.type();
    std::string name = value.name();

    if (!std::filesystem::exists(config_path))
        Logger::error("File not found. Make sure to call the init function before this.");

    // Defines an errorCode variable
    XMLError errorCode;

    // Defines a doc variable and attempts to load the file
    XMLDocument doc;
    errorCode = doc.LoadFile(config_path.c_str());

    if (errorCode == 0) // If doc.LoadFile succeded 
    {

        XMLElement *root = doc.RootElement(); // We determine the root element (null or an actual element)

        if (!root) // If root is null (which means a new file)
        {
            // New root element
            XMLNode *pRoot = doc.NewElement("brls:StorageFile");
            doc.InsertFirstChild(pRoot);

            // New child element
            XMLElement *element = doc.NewElement("brls:Property");

            element->SetAttribute("name", name.c_str());
            if (is_same_type<T, std::string>().result == true)
                element->SetAttribute("value", objectFromValue.c_str());
            else
                element->SetAttribute("value", otherToChar<T>(objectFromValue));
            element->SetAttribute("type", type.c_str());
            pRoot->InsertFirstChild(element);

            // We try to save the file
            doc.SaveFile(config_path.c_str());

            if (errorCode == 0) // If it succedded
                return true;
            else { // Otherwise
                Logger::error("TinyXML2 could not save the file. Error code {}.", std::to_string(errorCode));
                return false;
            }

        } else { // Otherwise if there is a root element
            for (XMLElement *e = root->FirstChildElement(); e != NULL;
                    e = e->NextSiblingElement()) {

                    if (e == nullptr) {
                            Logger::debug("nullptr???");
                            break;
                    }

                    if (strcmp(e->Attribute("name"), name.c_str()) == 0 && overwriteExisting) {
                        Logger::debug("Found another element with the same name as this new element. Overwrite Existing is enabled (by default).");
                        root->DeleteChild(e);
                        break;
                    } else if (!overwriteExisting) {
                        Logger::debug("Found another element with the same name as this new element. Overwrite Existing is disabled.");
                        return false;
                    }
            }
            

            // New child element
            XMLElement *element = doc.NewElement("brls:Property");

            element->SetAttribute("name", name.c_str());
            if (is_same_type<T, std::string>().result == true)
                element->SetAttribute("value", objectFromValue.c_str());
            else
                element->SetAttribute("value", otherToChar<T>(objectFromValue));
            element->SetAttribute("type", type.c_str());
            root->InsertFirstChild(element);

            // We try to save the file
            doc.SaveFile(config_path.c_str());

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

/*
 * In short, this function parses a certain XML element into a
 * element for the allStorageObjects vector. The certain XML element
 * is determined by its name attribute, since that attribute is like an id
 */
bool parseXMLToVector(std::string name) 
{
    XMLError errorCode;
    XMLDocument doc;
    errorCode = doc.LoadFile(config_path.c_str());

    if (errorCode == 0)
    {
        XMLNode *root = doc.RootElement();

        if (root == nullptr) {
            Logger::error("NULL root element. This means the XML file is blank (hasn't been written to yet), or the XML file is broken.");
            return false;
        }

        XMLElement *element = root->FirstChildElement();

        for (XMLElement *e = root->FirstChildElement(); e != NULL;
            e = e->NextSiblingElement()) {

            if (e == nullptr) {
                Logger::debug("nullptr???");
                break;
            }

            if (e->Attribute("name") == name.c_str()) {
                element = e;
            }
        }

        if (element != nullptr) { // TODO: Find out why this is always true (are two elements with the same attribute value freaking out the program?)
            const char * valueOfValue;
            const char * valueOfType;

            element->QueryStringAttribute("value", &valueOfValue);
            element->QueryStringAttribute("type", &valueOfType);

            StorageObject<T> obj{};

            obj.setName(name);
            obj.setType(std::string(valueOfType));

            char * newValueOfValue = new char[100];
            std::strcpy(newValueOfValue, valueOfValue);
            T actualVal = charToOther<T>(newValueOfValue);
            obj.setValue(actualVal);

            allStorageObjects.push_back(obj);
            return true;
        } else {
            Logger::warning("NULL element variable. This means there is a root element, but no child elements.");
            return false;
        }

    } else {
        Logger::error("TinyXML2 could not open the file. Error code {}.", std::to_string(errorCode));
        Logger::error("More details: {}", doc.ErrorStr());
        return false;
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
    bool result = parseXMLToVector(name);

    if (result) {
        size_t numberElement{ 0 }; // Defines numberElement for the return

        for (size_t i{ 0 }; i < allStorageObjects.size(); i++) { // For loop to loop through all elements of the std::vector
            auto currentElement = allStorageObjects[i]; // Set current element 

            if (currentElement.name() == name) { // If the current element name is equal to the name we are looking for
                numberElement = i; // Set numberElement to i and break the loop
                break;
            }
        }

        return allStorageObjects[numberElement]; // Return the StorageObject object from the std::vector
    } else {
        StorageObject<T> nullStorageObject;
        nullStorageObject.setIsEmpty(true);
        return nullStorageObject;
    }
}

/*
 * Allows you to check if the file is empty or not.
 * Returns true if it's empty, false if it's not 
 */
bool isFileEmpty() {
    XMLDocument doc;
    doc.LoadFile(config_path.c_str());

    XMLElement *root = doc.RootElement();
    if (!root) {
        return true;
    }
    else {
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

void setConfigPath(std::string filename) {
    this->filename = filename + ".xml";
    config_path = config_folder + this->filename;
}

#ifdef __SWITCH__ // If the client is running on a Switch, this approach is used
std::string config_folder = std::string("/config/") + "brls/appname"_i18n + "/";
#else // Otherwise, we assume that the client is running on a PC.
std::string config_folder = std::string("./config/") + "brls/appname"_i18n + "/";
#endif
std::string filename;

std::string config_path; // This is blank until Init function is called

};


// This is a more simple version of the BasicStorageFile class
// It includes functions that will improve how you use StorageFiles
// This class is a child class of a BasicStorageFile, so you can use all of those commands
// from BasicStorageFile, here.
class StorageFile : public BasicStorageFile<std::string>
{
    public:

    /*
    * A function that either writes data to the Storage File, or reads data depending on if the file is empty or not.
    * The first argument is for a vaild, not-empty brls::StorageObject variable, 
    * the second is for a possibly empty brls::StorageObject variable.
    */
    bool setup(StorageObject<std::string>& obj1, StorageObject<std::string>& obj2, std::string name) {
        if (this->isFileEmpty()) {
            this->writeToFile(obj1);
        } else {
            obj2 = this->readFromFile(name);
        }

        return true;
    }

    /*
    * A function that can save your changes to the Storage File.
    */ 
    bool save(StorageObject<std::string>& obj) {
        bool success = this->writeToFile(obj);
        return success;
    }

    /*
    * A function that can grab a value from the Storage File.
    */
    bool grab(StorageObject<std::string>& obj, std::string name) {
        obj = this->readFromFile(name);
        return true;
    }

};

} // namespace brls