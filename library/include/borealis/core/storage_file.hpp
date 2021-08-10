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

#include <borealis/core/util.hpp>
#include <borealis/core/logger.hpp>
#include <filesystem>
#include <fstream>
#include <string>

namespace brls
{

#define BRLS_STORAGE_FILE_INIT(classname, filename, appname) classname() {this->init(filename, appname);}

#define BRLS_STORAGE_INT(var, name) brls::IntStorageObject var = brls::IntStorageObject(name, this)
#define BRLS_STORAGE_FLOAT(var, name) brls::FloatStorageObject var = brls::FloatStorageObject(name, this)
#define BRLS_STORAGE_BOOL(var, name) brls::BoolStorageObject var = brls::BoolStorageObject(name, this)
#define BRLS_STORAGE_STRING(var, name) brls::StringStorageObject var = brls::StringStorageObject(name, this)

struct StorageFile;

/**
 * A superclass for all StorageObject types.
 */
template <typename T>
class StorageObject
{
    T value;
    std::string name;
    StorageFile *parent = nullptr;

    public:

    StorageObject(std::string name, StorageFile *newParent);
    virtual ~StorageObject() {};

    void setValue(T val);
    T getValue();
    void setName(std::string name);
    std::string getName();

    void setParent(StorageFile *newParent)
    {
        if (parent)
            parent = nullptr;

        parent = newParent;
    }

    StorageFile* getParent()
    {
        return parent;
    }

    void save();

    virtual std::string getTypeName() = 0;
};

template <typename T>
StorageObject<T>::StorageObject(std::string name, StorageFile *newParent)
{
    setName(name);
    setParent(newParent);
}

template <typename T>
void StorageObject<T>::setValue(T val)
{
    value = val;
}

template <typename T>
T StorageObject<T>::getValue()
{
    return value;
}

template <typename T>
void StorageObject<T>::setName(std::string name)
{
    this->name = name;
}

template <typename T>
std::string StorageObject<T>::getName()
{
    return name;
}

struct IntStorageObject : public StorageObject<int>
{
    using StorageObject<int>::StorageObject;
    std::string getTypeName() override;
};

struct FloatStorageObject : public StorageObject<float>
{
    using StorageObject<float>::StorageObject;
    std::string getTypeName() override;
};

struct BoolStorageObject : public StorageObject<bool>
{
    using StorageObject<bool>::StorageObject;
    std::string getTypeName() override;
};

struct StringStorageObject : public StorageObject<std::string>
{
    using StorageObject<std::string>::StorageObject;
    std::string getTypeName() override;
};

struct StorageFile
{
     bool inited = false;

    /**
     * Initalizes the Storage File. Required in order to use
     * the Storage File.
     */
    bool init(std::string filename, std::string appname);

    /**
     * Writes a storage object to the storage file.
     * Usually this is called by a storage object when the save function is called.
     */
    template <typename T>
    bool writeToFile(StorageObject<T> &object);
    template <typename T>
    bool readFromFile(std::string name, StorageObject<T> &object);

    private:
    std::string config_path;
    std::string filename;
};

template <typename T>
bool StorageFile::writeToFile(StorageObject<T> &object)
{
    const char *value = ConversionUtils::toCString(object.getValue());
    std::string name = object.getName();

    if (!inited)
    {
        Logger::error("StorageFile with the filename {} has not been initalized yet.", this->filename);
        return false;
    }

    tinyxml2::XMLDocument doc;
    tinyxml2::XMLError errorCode = doc.LoadFile(config_path.c_str());

    tinyxml2::XMLElement *root = doc.RootElement();
    if (!root || errorCode == tinyxml2::XML_ERROR_EMPTY_DOCUMENT)
    {
        tinyxml2::XMLNode *pRoot = doc.NewElement("brls:StorageFile");
        doc.InsertFirstChild(pRoot);

        tinyxml2::XMLElement *element = doc.NewElement(("brls:" + object.getTypeName() + "Property").c_str());

        element->SetAttribute("name", name.c_str());
        element->SetAttribute("value", value);
        pRoot->InsertEndChild(element);

        errorCode = doc.SaveFile(config_path.c_str());

        if (errorCode == tinyxml2::XML_SUCCESS || errorCode == tinyxml2::XML_ERROR_EMPTY_DOCUMENT)
            return true;
        else
        {
            Logger::error("TinyXML2 could not save the file. Error code {}.", std::to_string(errorCode));
            Logger::error("More details: {}", doc.ErrorStr());
            return false;
        }
    }
    else if (errorCode == tinyxml2::XML_SUCCESS)
    {
        for (tinyxml2::XMLElement *e = root->FirstChildElement(); e != NULL; e = e->NextSiblingElement())
        {
            if (std::strcmp(e->Attribute("name"), name.c_str()) == 0)
            {
                root->DeleteChild(e);
                break;
            }
        }

        tinyxml2::XMLElement *element = doc.NewElement(("brls:" + object.getTypeName() + "Property").c_str());

        element->SetAttribute("name", name.c_str());
        element->SetAttribute("value", value);
        root->InsertEndChild(element);

        errorCode = doc.SaveFile(config_path.c_str());

        if (errorCode == tinyxml2::XML_SUCCESS || errorCode == tinyxml2::XML_ERROR_EMPTY_DOCUMENT)
            return true;
        else
        {
            Logger::error("TinyXML2 could not save the file. Error code {}.", std::to_string(errorCode));
            Logger::error("More details: {}", doc.ErrorStr());
            return false;
        }
    }
    
    Logger::error("TinyXML2 could not open the file. Error code {}.", std::to_string(errorCode));
    Logger::error("More details: {}", doc.ErrorStr());
    return false;
}

template <typename T>
bool StorageFile::readFromFile(std::string name, StorageObject<T> &object)
{
    if (!inited)
    {
        Logger::error("StorageFile with the filename {} has not been initalized yet.", this->filename);
        return false;
    }

    tinyxml2::XMLDocument doc;
    tinyxml2::XMLError errorCode = doc.LoadFile(config_path.c_str());

    if (errorCode == tinyxml2::XML_SUCCESS)
    {
        tinyxml2::XMLElement *root = doc.RootElement();
        if (root == nullptr)
        {
            Logger::error("StorageFile: NULL root element. This means the XML file is blank (hasn't been written to yet), or the XML file is broken.");
            return false;
        }

        tinyxml2::XMLElement *element = nullptr;
        for (tinyxml2::XMLElement *e = root->FirstChildElement(); e != NULL; e = e->NextSiblingElement())
        {
            if (!e)
                break;

            if (std::strcmp(e->Attribute("name"), name.c_str()) == 0)
            {
                element = e;
                break;
            }
        }

        if (element)
        {
            const char *value;
            element->QueryStringAttribute("value", &value);

            object.setName(name);
            object.setValue(ConversionUtils::fromCString<T>(value));

            return true;
        }
        else
        {
            Logger::error("StorageFile: Could not find property with the name {}.", name);
            return false;
        }
    }
    
    Logger::error("TinyXML2 could not open the file. Error code {}.", std::to_string(errorCode));
    Logger::error("More details: {}", doc.ErrorStr());
    return false;
}

template <typename T>
void StorageObject<T>::save()
{
    if (parent)
    {
        parent->writeToFile(*this);
    }
}

} // namespace brls