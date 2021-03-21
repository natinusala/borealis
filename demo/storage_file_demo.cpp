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

    settingsObject.setValue((char*) "Hello! I'm coming soon to the release of Borealis!");
    settings->save(settingsObject);

    settingsObject.setName("Boolean Test");
    settingsObject.setType("bool");
    settingsObject.setValue(brls::ConversionUtils::toCString<bool>(true));
    settings->save(settingsObject);

    listSettingsObject.setName("List Demo");
    listSettingsObject.setType("char*");
    listSettingsObject.push_back((char*) "Hello!");
    listSettingsObject.push_back((char*) "I'm coming to the release of");
    listSettingsObject.push_back((char*) "Borealis!");

    settings->save(listSettingsObject);

    delete settings;
    return true;
}

bool StorageFileDemo::onReadDataButtonPressed(brls::View *view) {
    SettingsFile *settings = new SettingsFile();

    settings->grab(burnerObject, "Demo");
    if (!burnerObject.getIsEmpty())
        settingsObject = burnerObject;
    else return false;
    std::cout << "First value from the XML File: " << settingsObject.value() << '\n';

    settings->grab(burnerObject, "Boolean Test");
    if (!burnerObject.getIsEmpty())
        settingsObject = burnerObject;
    else return false;
    bool res = brls::ConversionUtils::fromCString<bool>(settingsObject.value());
    std::cout << "Second value from the XML File (converted back to a bool): " << std::boolalpha << res << '\n';
    
    settings->grab(listBurnerObject, "List Demo");
    if (!listBurnerObject.getIsEmpty())
        listSettingsObject = listBurnerObject;
    else return false;
    std::cout << "Last value from the list from the XML File: " << listSettingsObject[2] << '\n';

    delete settings;
    return true;
}

brls::View* StorageFileDemo::create() {
    return new StorageFileDemo();
}