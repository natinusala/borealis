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

// Note: This file, it's header counterpart, and any other files relating to the Storage File Demo can be taken down at demand.
// They are only here for testing purposes.

#include "storage_file_demo.hpp"

StorageFileDemo::StorageFileDemo() {
    this->inflateFromXMLRes("xml/tabs/storage_file.xml");

    BRLS_REGISTER_CLICK_BY_ID("writeData", this->onWriteDataButtonPressed);
    BRLS_REGISTER_CLICK_BY_ID("readData", this->onReadDataButtonPressed);
}

bool StorageFileDemo::onWriteDataButtonPressed(brls::View *view) {
    SettingsFile *settings = new SettingsFile();
    settingsObject.setName("Demo");
    settingsObject.setType("char*");

    const char *const_val = "Hello! I'm coming soon to the release of Borealis!";
    settingsObject.setValue((char*) const_val);
    settings->save(settingsObject);
    return true;
}

bool StorageFileDemo::onReadDataButtonPressed(brls::View *view) {
    SettingsFile *settings = new SettingsFile();
    settings->grab(burnerObject, "Demo");

    if (!burnerObject.getIsEmpty())
        settingsObject = burnerObject;
    else return false;

    auto valueFromXML = settingsObject.value();

    std::cout << "Value from the XML File: " << valueFromXML << std::endl;
    return true;
}

brls::View* StorageFileDemo::create() {
    return new StorageFileDemo();
}