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
#include <iostream>
#include <fstream>
#include <filesystem>
//#include <string.h>
#include <string>
#include <vector>
#include <map>
#include <borealis/core/logger.hpp>
#include <borealis/core/i18n.hpp>

using namespace brls::literals;
using namespace tinyxml2;

namespace brls 
{

// Macros (more will be added once the whole Storage system is simpilified)
#define BRLS_STORAGE_FILE_INIT(filename) bool initSuccess = this->init(filename)

#define BRLS_STORAGE_FILE_WRITE_DATA(StorageObject, variableBool) variableBool = this->writeToFile(StorageObject)
#define BRLS_STORAGE_FILE_READ_DATA(StorageObject, name) StorageObject = this->readFromFile(name)

#define BRLS_STORAGE_FILE_WRITE_LIST_DATA(ListStorageObject, variableBool) variableBool = this->writeListToFile(ListStorageObject)
#define BRLS_STORAGE_FILE_READ_LIST_DATA(ListStorageObject, name) ListStorageObject = this->readListFromFile(name)

#define BRLS_STORAGE_OBJECT(variableName, value, name, type) brls::StorageObject variableName = brls::StorageObject(value, name, type)
#define BRLS_BLANK_STORAGE_OBJECT(variableName) brls::StorageObject variableName = brls::StorageObject()
#define BRLS_LIST_STORAGE_OBJECT(variableName, value, name, type) brls::ListStorageObject variableName = brls::ListStorageObject(name, value, type)
#define BRLS_BLANK_LIST_STORAGE_OBJECT(variableName) brls::ListStorageObject variableName = brls::ListStorageObject()

// This is a StorageObject, which is a data type of a value stored onto the XML file.
// They have two variables they hold onto throughout their lifetime: the value and name.
// They also have an std::string of their type, since the value will be stored as a char *
// In the XML File
class StorageObject 
{

    public:

    StorageObject() {}
    StorageObject(char *val, std::string name, std::string type) {
        valueSet = new char[sizeof(val) + 1];
        this->setValue(val);
        this->setName(name); 
        this->setType(type);
    }

    void setValue(char *newValue) {
        valueSet = newValue;
    }

    char* value() {
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
    char *valueSet;
    std::string nameInXML;
    std::string typeOfValue;

    bool isEmpty = false;

};

// This is a ListStorageObject, which is similar to a normal StorageObject, but a whole lot different.
// Instead of only holding one value at a time, ListStorageObjects hold any amount of values.
// These lists contain Values, which are normal StorageObjects that only hold a value. No names or type is stored.
class ListStorageObject
{
    struct Value { char *val; }; // POD Struct (Plain Old Data-type)

    public:

    ListStorageObject() {}
    ListStorageObject(std::string name, char *value, std::string type) {
        this->push_back(value);
        this->setName(name);
        this->setType(type);
    }

    void push_back(char *newVal) {
        Value newPOD = Value{};
        newPOD.val = newVal;
        values.push_back(newPOD);
    }

    void push_front(char *newVal) {
        Value newPOD = Value{};
        newPOD.val = newVal;
        values.insert(values.begin(), newPOD);
    }

    void insert(size_t index, char *newVal) {
        Value newPOD = Value{};
        newPOD.val = newVal;
        values.insert(values.begin() + index, newPOD);
    }

    char* get_front() {
        auto valFromVec = values[0];
        return valFromVec.val;
    }

    char* get_back() {
        auto valFromVec = values[values.size() - 1];
        return valFromVec.val;
    }

    char* get(size_t index) {
        auto valFromVec = values[index];
        return valFromVec.val;
    }

    char* operator[](size_t index) {
        return this->get(index);
    }

    bool operator==(const char *other) {
        auto elem = this->get_back();
        return std::strcmp(elem, other) == 0;
    }

    const size_t size() {
        return values.size();
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

    public:

    /*
     * Inits the config folder if it doesn't exist and creates a file.
     * Remember to not include the .xml part in the filename argument, it's already added.
     * If you don't, tinyxml2 will look for file (.)/config/(appname here)/(filename here).xml.xml later on.
     */
    bool init(std::string filename) {
        #ifndef __SWITCH__
        config_folder = std::filesystem::current_path().string() + "/config/" + "brls/appname"_i18n + "/";
        #else
        config_folder = "/config/" + "brls/appname"_i18n + "/";
        #endif

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
     *      <brls:Property name="wizard_shown" value="true" type="bool"/>
     *	    <brls:Property name="username" value="h4ck3rm4n" type="std::string"/>
     *   </brls:StorageFile>
     * 
     * It gives a fatal error if the filename is not found (call the init function before these functions).
     */
    bool writeToFile(StorageObject value, bool overwriteExisting = true)
    {   
            // Gets values from the StorageObject and configures the config_path variable
            const char *objectFromValue = value.value();
            std::string type = value.type();
            std::string name = value.name();

            if (!std::filesystem::exists(config_path))
                Logger::error("File not found. Make sure to call the init function before this.");

            // Defines an errorCode variable
            XMLError errorCode;

            // Defines a doc variable and attempts to load the file
            XMLDocument doc;
            errorCode = doc.LoadFile(config_path.c_str());

            XMLElement *root = doc.RootElement(); // We determine the root element (null or an actual element)

            if (!root || errorCode == XML_ERROR_EMPTY_DOCUMENT) // If root is null or LoadFile returned the XML_ERROR_EMPTY_DOCUMENT error (which means a new file)
            {
                // New root element
                XMLNode *pRoot = doc.NewElement("brls:StorageFile");
                doc.InsertFirstChild(pRoot);

                // New child element
                XMLElement *element = doc.NewElement("brls:Property");

                element->SetAttribute("name", name.c_str());
                element->SetAttribute("value", objectFromValue);
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

            } else if (errorCode != XML_ERROR_EMPTY_DOCUMENT) { // Otherwise if there is a root element or LoadFile didn't return an error
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
                element->SetAttribute("value", objectFromValue);
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
            } else { // Else doc variable had other troubles loading the document
                Logger::error("TinyXML2 could not open the file. Error code {}.", std::to_string(errorCode));
                Logger::error("More details: {}", doc.ErrorStr());
                return false;
            }
    }

    /*
     * Writes a whole list to the XML File.
     * Similar to writeToFile.
     */ 
    bool writeListToFile(ListStorageObject obj, bool overwriteExisting = true)
    {
        std::string name = obj.name();
        std::string type = obj.type();

        if (!std::filesystem::exists(config_path))
                Logger::error("File not found. Make sure to call the init function before this.");

        // Defines an errorCode variable
        XMLError errorCode;

        // Defines a doc variable and attempts to load the file
        XMLDocument doc;
        errorCode = doc.LoadFile(config_path.c_str());

        XMLElement *root = doc.RootElement(); // We determine the root element (null or an actual element)

        if (!root || errorCode == XML_ERROR_EMPTY_DOCUMENT) // If root is null or LoadFile returned the XML_ERROR_EMPTY_DOCUMENT error (which means a new file)
        {

            // New root element
            XMLNode *pRoot = doc.NewElement("brls:StorageFile");
            doc.InsertFirstChild(pRoot);

            // New child element
            XMLElement *element = doc.NewElement("brls:ListProperty");

            element->SetAttribute("name", name.c_str());  
            element->SetAttribute("type", type.c_str());
            for (size_t i{0}; i < obj.size(); i++) {
                XMLElement *listElement = doc.NewElement("brls:Value");
                listElement->SetAttribute("value", obj[i]);
                element->InsertEndChild(listElement);
            }
            pRoot->InsertFirstChild(element);            

            // We try to save the file
            doc.SaveFile(config_path.c_str());

            if (errorCode == 0) // If it succedded
                return true;
            else { // Otherwise
                Logger::error("TinyXML2 could not save the file. Error code {}.", std::to_string(errorCode));
                Logger::error("More details: {}", doc.ErrorStr());
                return false;
            }

        } else if (errorCode != XML_ERROR_EMPTY_DOCUMENT) { // Otherwise if there is a root element or LoadFile didn't return an error

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
            XMLElement *element = doc.NewElement("brls:ListProperty");

            element->SetAttribute("name", name.c_str());  
            element->SetAttribute("type", type.c_str());
            for (size_t i{0}; i < obj.size(); i++) {
                XMLElement *listElement = doc.NewElement("brls:Value");
                listElement->SetAttribute("value", obj[i]);
                element->InsertEndChild(listElement);
            }
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

        } else { // Else doc variable had other troubles loading the document
            Logger::error("TinyXML2 could not open the file. Error code {}.", std::to_string(errorCode));
            Logger::error("More details: {}", doc.ErrorStr());
            return false;
        }
    }


    /*
     * In short, this function parses a certain XML element into a
     * element for the allStorageObjects Map. The certain XML element
     * is determined by its name attribute, since that attribute is like an id
     */
    bool parseXMLToMap(std::string name)
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
                    if (std::strcmp(e->Attribute("name"), name.c_str()) == 0) {
                        element = e;
                    }
                }

                if (element != nullptr) {
                    const char *valueOfValue;
                    const char *valueOfType;

                    element->QueryStringAttribute("value", &valueOfValue);
                    element->QueryStringAttribute("type", &valueOfType);

                    StorageObject obj{};

                    obj.setName(name);
                    obj.setType(std::string(valueOfType));

                    char *val = new char[sizeof(valueOfValue) + 1];
                    std::strcpy(val, valueOfValue);
                    obj.setValue(val);

                    allStorageObjectsMap[name] = obj;
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

    bool parseXMLToListMap(std::string name)
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

                XMLElement *element = root->FirstChildElement("brls:ListProperty");

                for (XMLElement *e = root->FirstChildElement("brls:ListProperty"); e != NULL;
                    e = e->NextSiblingElement("brls:ListProperty")) {

                    if (e == nullptr) {
                        Logger::debug("nullptr???");
                        break;
                    }
                    if (std::strcmp(e->Attribute("name"), name.c_str()) == 0) {
                        element = e;
                    }
                }

                if (element != nullptr) {
                    ListStorageObject obj{};
                    obj.setName(name);

                    const char *valueOfValue;
                    const char *valueOfType;

                    element->QueryStringAttribute("type", &valueOfType);
                    obj.setType(std::string(valueOfType));

                    for (XMLElement *e = element->FirstChildElement("brls:Value"); e != NULL;
                        e = e->NextSiblingElement("brls:Value")) {
                            char *currentAttributeElem = new char[100];
                            e->QueryStringAttribute("value", (const char**) &currentAttributeElem);
                            obj.push_back(currentAttributeElem);
                    }

                    allListStorageObjectsMap[name] = obj;
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
    StorageObject readFromFile(std::string name)
    {
        bool result = this->parseXMLToMap(name);

        if (result) {
            StorageObject obj;

            for (const auto& x : allStorageObjectsMap) {
                if (x.first == name) {
                    obj = x.second;
                    break;
                }
            }

            return obj;
        } else {
            StorageObject nullStorageObject{};
            nullStorageObject.setIsEmpty(true);
            return nullStorageObject;
        }
    }

    ListStorageObject readListFromFile(std::string name)
    {
        bool result = this->parseXMLToListMap(name);

        if (result) {
            ListStorageObject obj;

            for (const auto& x : allListStorageObjectsMap) {
                if (x.first == name) {
                    obj = x.second;
                    break;
                }
            }

            return obj;
        } else {
            ListStorageObject nullStorageObject{};
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

    private:

    void setConfigPath(std::string filename) {
        this->filename = filename + ".xml";
        config_path = config_folder + this->filename;
    }

    std::string config_folder;
    std::string config_path;
    std::string filename;
};

class StorageFile : public BasicStorageFile 
{
    public:

    /*
     * Saves any changes you made to a StorageObject to the XML file. Returns a bool if it succeded or not
     */
    bool save(StorageObject& obj) {
        bool success; 
        BRLS_STORAGE_FILE_WRITE_DATA(obj, success);
        return success;
    }

    bool save(ListStorageObject& obj) {
        bool success;
        BRLS_STORAGE_FILE_WRITE_LIST_DATA(obj, success);
        return success;
    }

    /*
     * Reads a certain element from the XML file and returns it to a StorageObject. Returns a bool if it succeded or not
     */
    bool grab(StorageObject& obj, std::string name) {
        BRLS_STORAGE_FILE_READ_DATA(obj, name);
        if (!obj.getIsEmpty()) return true;
        else return false;
    }

    bool grab(ListStorageObject& obj, std::string name) {
        BRLS_STORAGE_FILE_READ_LIST_DATA(obj, name);
        if (!obj.getIsEmpty()) return true;
        else return false;
    }

};

} // namespace brls