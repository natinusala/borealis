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

#include <borealis/core/i18n.hpp>
#include <borealis/core/logger.hpp>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <vector>

using namespace brls::literals;
using namespace tinyxml2;

namespace brls
{

// Macros (more will be added once the whole Storage system is simpilified)
#define BRLS_STORAGE_FILE_INIT(filename, appname) bool initSuccess = this->init(filename, appname)

#define BRLS_STORAGE_OBJECT(variableName, value, name, type) brls::StorageObject variableName = brls::StorageObject(value, name, type)
#define BRLS_BLANK_STORAGE_OBJECT(variableName) brls::StorageObject variableName = brls::StorageObject()
#define BRLS_LIST_STORAGE_OBJECT(variableName, value, name, type) brls::ListStorageObject variableName = brls::ListStorageObject(name, value, type)
#define BRLS_BLANK_LIST_STORAGE_OBJECT(variableName) brls::ListStorageObject variableName = brls::ListStorageObject()

// This is a StorageObject, which is a data type of a value stored onto the XML file.
// They have two variables they hold onto throughout their lifetime: the value and name.
// They also have an std::string of their type, since the value will be stored as a char *
// In the XML File
struct StorageObject
{
    StorageObject() {}
    StorageObject(char* val, std::string name, std::string type);

    void setValue(char* newValue);
    char* value();
    void setName(std::string val);
    std::string name();
    void setType(std::string newType);
    std::string type();
    void setIsEmpty(bool newVal);
    bool getIsEmpty();

  private:
    char* valueSet;
    std::string nameInXML;
    std::string typeOfValue;

    bool isEmpty = false;
};

// This is a ListStorageObject, which is similar to a normal StorageObject, but a whole lot different.
// Instead of only holding one value at a time, ListStorageObjects hold any amount of values.
// These lists contain Values, which are normal StorageObjects that only hold a value. No names or type is stored.
struct ListStorageObject
{
    struct Value
    {
        char* val;
    }; // POD Struct (Plain Old Data-type)

    ListStorageObject() {}
    ListStorageObject(std::string name, char* value, std::string type);

    void push_back(char* newVal);
    void push_front(char* newVal);
    void insert(size_t index, char* newVal);
    char* get_front();
    char* get_back();
    char* get(size_t index);
    char* operator[](size_t index);
    bool operator==(const char* other);
    bool operator!=(const char* other);
    const size_t size();
    std::vector<Value>::iterator begin();
    std::vector<Value>::iterator end();

    void setName(std::string val);
    std::string name();
    void setType(std::string newType);
    std::string type();
    void setIsEmpty(bool newVal);
    bool getIsEmpty();

  private:
    std::vector<Value> values;
    std::string nameInXML;
    std::string typeOfValue;

    bool isEmpty = false;
};

// The StorageFile class allows you to create, write, or read from files for anything
// For example, you can use it for a config or track when a user last used the app.
class BasicStorageFile
{

    std::map<std::string, StorageObject> allStorageObjectsMap; // Main map where all Storage Objects are stored
    std::map<std::string, ListStorageObject> allListStorageObjectsMap; // Main map where all List Storage Objects are stored

    StorageObject *currentStorageObject = nullptr;
    ListStorageObject *currentListStorageObject = nullptr;

  public:
    /*
     * Inits the config folder if it doesn't exist and creates a file.
     * Remember to not include the .xml part in the filename argument, it's already added.
     */
    bool init(std::string filename, std::string appname);

    /*
     * Writes a value to the storage file.
     * 
     * An example would be the following: 
     *
     * <brls:Property name="wizard_shown" value="true" type="bool"/>
     * <brls:Property name="username" value="h4ck3rm4n" type="std::string"/>
     * 
     * It gives a fatal error if the filename is not found (call the init function before these functions).
     */
    bool writeToFile(StorageObject& value, bool overwriteExisting = true);

    /*
     * Writes a whole list to the XML File.
     * Similar to writeToFile.
     */
    bool writeListToFile(ListStorageObject& obj, bool overwriteExisting = true);

    /*
     * In short, this function parses a certain XML element into a
     * element for the allStorageObjects Map. The certain XML element
     * is determined by its name attribute, since that attribute is like an id
     */
    bool parseXMLToObject(std::string name);

    /*
     * In short, this function parses a certain XML element into a
     * element for the allStorageObjects Map. The certain XML element
     * is determined by its name attribute, since that attribute is like an id
     */
    bool parseXMLToListObject(std::string name);

    /*
     * Reads a value from the config file, then returns a variable pointing to that value.
     * 
     * For example, if you store a variable with the data "EmreTech is awesome" into a storage file,
     * This function will find that value and return it, so you can read/change the value throughout
     * the program running.
     */
    StorageObject readFromFile(std::string name);

    ListStorageObject readListFromFile(std::string name);

    /*
     * Allows you to check if the file is empty or not.
     * Returns true if it's empty, false if it's not 
     */
    bool isFileEmpty();

  private:
    void setConfigPath(std::string filename)
    {
        this->filename = filename + ".xml";
        config_path    = config_folder + this->filename;
    }

    std::string config_folder;
    std::string config_path;
    std::string filename;
};

// An easier way to use brls::BasicStorageFile. No confusing functions.
// StorageFile is a child class to brls::BasicStorageFile, so you can use
// all of those functions, here.
struct StorageFile : public BasicStorageFile
{
    /*
     * Saves any changes you made to a StorageObject to the XML file. Returns a bool if it succeded or not
     */
    bool save(StorageObject& obj);

    /*
     * Saves any changes you made to a StorageObject to the XML file. Returns a bool if it succeded or not
     */
    bool save(ListStorageObject& obj);

    /*
     * Reads a certain element from the XML file and returns it to a StorageObject. Returns a bool if it succeded or not
     */
    bool grab(StorageObject& obj, std::string name);

    /*
     * Reads a certain element from the XML file and returns it to a StorageObject. Returns a bool if it succeded or not
     */
    bool grab(ListStorageObject& obj, std::string name);
};

} // namespace brls