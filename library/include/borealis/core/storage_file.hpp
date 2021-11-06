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
#include <string>
#include <vector>

namespace brls
{

#define BRLS_STORAGE_FILE_INIT(classname, filename, appname) classname() {this->init(filename, appname);}

#define BRLS_STORAGE_INT(var, name) brls::IntStorageObject var = brls::IntStorageObject(name, this)
#define BRLS_STORAGE_FLOAT(var, name) brls::FloatStorageObject var = brls::FloatStorageObject(name, this)
#define BRLS_STORAGE_BOOL(var, name) brls::BoolStorageObject var = brls::BoolStorageObject(name, this)
#define BRLS_STORAGE_STRING(var, name) brls::StringStorageObject var = brls::StringStorageObject(name, this)

#define BRLS_STORAGE_INT_LIST(var, name) brls::IntListStorageObject var = brls::IntListStorageObject(name, this)
#define BRLS_STORAGE_FLOAT_LIST(var, name) brls::FloatListStorageObject var = brls::FloatListStorageObject(name, this)
#define BRLS_STORAGE_BOOL_LIST(var, name) brls::BoolListStorageObject var = brls::BoolListStorageObject(name, this)
#define BRLS_STORAGE_STRING_LIST(var, name) brls::StringListStorageObject var = brls::StringListStorageObject(name, this)

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

    void setValue(const T val);
    T getValue();
    void setName(std::string name);
    std::string getName();
    void setParent(StorageFile *newParent);
    StorageFile* getParent();

    void operator=(const T val);

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
void StorageObject<T>::setValue(const T val)
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

template <typename T>
void StorageObject<T>::setParent(StorageFile *newParent)
{
    if (parent)
        parent = nullptr;
    
    parent = newParent;
}

template <typename T>
StorageFile* StorageObject<T>::getParent()
{
    return parent;
}

template <typename T>
void StorageObject<T>::operator=(const T val)
{
    this->setValue(val);
}

template <typename T>
class ListStorageObject
{
    std::vector<T> values;
    std::string name;
    StorageFile *parent = nullptr;

    public:

    ListStorageObject(std::string name, StorageFile *newParent);
    virtual ~ListStorageObject() {};

    void pushValue(const T val);
    void insert(const T val, size_t index);
    T getValue(size_t index);
    size_t size();

    std::vector<T>& getVector();

    void setName(std::string name);
    std::string getName();
    void setParent(StorageFile *newParent);
    StorageFile* getParent();

    void save();

    virtual std::string getTypeName() = 0;
};

template <typename T>
ListStorageObject<T>::ListStorageObject(std::string name, StorageFile *newParent)
{
    setName(name);
    setParent(newParent);
}

template <typename T>
std::vector<T>& ListStorageObject<T>::getVector()
{
    return this->values;
}

template <typename T>
void ListStorageObject<T>::pushValue(const T val)
{
    values.push_back(val);
}

template <typename T>
void ListStorageObject<T>::insert(const T val, size_t index)
{
    values[index] = val;
}

template <typename T>
T ListStorageObject<T>::getValue(size_t index)
{
    return values.at(index);
}

template <typename T>
size_t ListStorageObject<T>::size()
{
    return values.size();
}

template <typename T>
void ListStorageObject<T>::setName(std::string name)
{
    this->name = name;
}

template <typename T>
std::string ListStorageObject<T>::getName()
{
    return name;
}

template <typename T>
void ListStorageObject<T>::setParent(StorageFile *newParent)
{
    if (parent)
        parent = nullptr;
    
    parent = newParent;
}

template <typename T>
StorageFile* ListStorageObject<T>::getParent()
{
    return parent;
}

struct IntStorageObject : public StorageObject<int>
{
    using StorageObject<int>::StorageObject;
    using StorageObject<int>::operator=;
    std::string getTypeName() override;
};

struct FloatStorageObject : public StorageObject<float>
{
    using StorageObject<float>::StorageObject;
    using StorageObject<float>::operator=;
    std::string getTypeName() override;
};

struct BoolStorageObject : public StorageObject<bool>
{
    using StorageObject<bool>::StorageObject;
    using StorageObject<bool>::operator=;
    std::string getTypeName() override;
};

struct StringStorageObject : public StorageObject<std::string>
{
    using StorageObject<std::string>::StorageObject;
    using StorageObject<std::string>::operator=;
    std::string getTypeName() override;
};

struct IntListStorageObject : public ListStorageObject<int>
{
    using ListStorageObject<int>::ListStorageObject;
    std::string getTypeName() override;
};

struct FloatListStorageObject : public ListStorageObject<float>
{
    using ListStorageObject<float>::ListStorageObject;
    std::string getTypeName() override;
};

struct BoolListStorageObject : public ListStorageObject<bool>
{
    using ListStorageObject<bool>::ListStorageObject;
    std::string getTypeName() override;
};

struct StringListStorageObject : public ListStorageObject<std::string>
{
    using ListStorageObject<std::string>::ListStorageObject;
    std::string getTypeName() override;
};

struct StorageFile
{
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

    /**
     * Reads a saved storage object from the storage file.
     */
    template <typename T>
    bool readFromFile(std::string name, StorageObject<T> &object);

    /**
     * Writes a storage object to the storage file.
     * Usually this is called by a storage object when the save function is called.
     */
    template <typename T>
    bool writeToFile(ListStorageObject<T> &object);

    /**
     * Reads a saved storage object from the storage file.
     */
    template <typename T>
    bool readFromFile(std::string name, ListStorageObject<T> &object);

    private:
    bool inited = false;
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
bool StorageFile::writeToFile(ListStorageObject<T> &object)
{
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

        tinyxml2::XMLElement *element = doc.NewElement(("brls:" + object.getTypeName() + "ListProperty").c_str());

        element->SetAttribute("name", name.c_str());

        for (size_t i{0}; i < object.size(); i++)
        {
            tinyxml2::XMLElement *e = doc.NewElement("brls:Value");
            e->SetAttribute("value", brls::ConversionUtils::toCString(object.getValue(i)));
            element->InsertEndChild(e);
        }

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

        tinyxml2::XMLElement *element = doc.NewElement(("brls:" + object.getTypeName() + "ListProperty").c_str());

        element->SetAttribute("name", name.c_str());

        for (size_t i{0}; i < object.size(); i++)
        {
            tinyxml2::XMLElement *e = doc.NewElement("brls:Value");
            e->SetAttribute("value", brls::ConversionUtils::toCString(object.getValue(i)));
            element->InsertEndChild(e);
        }

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
        for (tinyxml2::XMLElement *e = root->FirstChildElement(("brls:" + object.getTypeName() + "Property").c_str()); e != NULL; e = e->NextSiblingElement(("brls:" + object.getTypeName() + "Property").c_str()))
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
bool StorageFile::readFromFile(std::string name, ListStorageObject<T> &object)
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
        for (tinyxml2::XMLElement *e = root->FirstChildElement(("brls:" + object.getTypeName() + "ListProperty").c_str()); e != NULL; e = e->NextSiblingElement(("brls:" + object.getTypeName() + "ListProperty").c_str()))
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
           for (tinyxml2::XMLElement *e = element->FirstChildElement("brls:Value"); e != NULL; e = e->NextSiblingElement("brls:Value"))
           {
               const char *attributeEle;
               e->QueryStringAttribute("value", &attributeEle);
               brls::Logger::debug("Value: {}", attributeEle);
               object.pushValue(brls::ConversionUtils::fromCString<T>(attributeEle));
           }
            object.setName(name);

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
        parent->writeToFile(*this);
}

template <typename T>
void ListStorageObject<T>::save()
{
    if (parent)
        parent->writeToFile(*this);
}

} // namespace brls