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

#include <string>
#include <iostream>
#include <borealis.hpp>

class SettingsFile : public brls::StorageFile<std::string> {
    BRLS_STORAGE_FILE_INIT("settings");

    public:
    BRLS_STORAGE_FILE_BLANK_STORAGE_OBJECT(testValue, std::string);

    void setup() {
        if (this->isFileEmpty()) {
            testValue = this->createStorageObject("Hi", "test", "std::string"); // In order, the arguments mean variableName, value, name, and type (in this case, std::string)
            brls::Logger::debug("Value: {} Name: {} Type: {}", testValue.value(), testValue.name(), testValue.type());
            this->writeToFile(testValue);
        } 
        else {     
            testValue = this->readFromFile("test");
        }
    }
};