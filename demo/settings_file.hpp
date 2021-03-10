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
    BRLS_STORAGE_FILE_STORAGE_OBJECT(testValue, "Hi", "test", "std::string"); // In order, the arguments mean variableName, value, name, and type (in this case, std::string)

    BRLS_STORAGE_FILE_READ_DATA(testValue, "test");

    void printValue() {
        auto val = this->testValue.value();
        std::cout << "Value from XML: " << val << std::endl;
    }
};