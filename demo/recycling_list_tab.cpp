/*
    Copyright 2020-2021 natinusala

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

#include "recycling_list_tab.hpp"

RecyclerCell::RecyclerCell()
{
    this->inflateFromXMLRes("xml/cells/cell.xml");
}

RecyclerCell* RecyclerCell::create()
{
    return new RecyclerCell();
}

// DATA SOURCE

int DataSource::numberOfRows()
{
    return 20;
}

brls::RecyclerCell* DataSource::cellForRow(brls::RecyclerFrame* recycler, int row)
{
    RecyclerCell* item = (RecyclerCell*)recycler->dequeueReusableCell("Cell");
    item->label->setText("Item #" + std::to_string(row));
    if (row == 7)
        item->label->setText("Item # JM FKNKLmf knlwkenf jkN Fklmew kfnmL Fl enfklMEK MlKWNMF lkem KmLK NefnlkMElkf mlkwemf lkMLKFEm KLMlkf mLKMF lkMSElk fmLKFM lkesb fhhsk gmNLKmkS fe");
    return item;
}

// RECYCLER VIEW

RecyclingListTab::RecyclingListTab()
{
    // Inflate the tab from the XML file
    this->inflateFromXMLRes("xml/tabs/recycling_list.xml");

    recycler->registerCell("Cell", []() { return RecyclerCell::create(); });
    recycler->setDataSource(new DataSource());

    recycler->addGestureRecognizer(new brls::TapGestureRecognizer([this](brls::TapGestureStatus status) {
        this->recycler->reloadData();
        return false;
    },
        true));
}

brls::View* RecyclingListTab::create()
{
    // Called by the XML engine to create a new RecyclingListTab
    return new RecyclingListTab();
}
