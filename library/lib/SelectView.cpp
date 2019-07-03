/*
    Borealis, a Nintendo Switch UI Library
    Copyright (C) 2019  natinusala

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include <SelectView.hpp>
#include <Application.hpp>

#include <Animations.hpp>

#define SELECT_VIEW_MAX_ITEMS 6 // for max height

#define min(a, b) ((a < b) ? a : b)

// TODO: Selection marker
// TODO: Default selection

SelectView::SelectView(string title, vector<string> values, SelectListener listener, int selected) :
    title(title),
    selectedValue(selected),
    listener(listener)
{
    Style *style = Application::getStyle();

    this->topOffset = (float)style->SelectView.listPadding / 8.0f;

    this->valuesCount = values.size();

    this->list = new List();
    this->list->setParent(this);
    this->list->setMargins(1, 0, 1, 0);

    for (size_t i = 0; i < values.size(); i++)
    {
        string value = values[i];

        ListItem *item = new ListItem(value);

        item->setHeight(style->SelectView.listItemHeight);
        item->setTextSize(style->SelectView.listItemTextSize);

        item->setClickListener([listener, i](View *view){
            if (listener)
                listener(i);
            Application::popView();
        });

        this->list->addView(item);
    }
}

void SelectView::show(function<void(void)> cb)
{
    View::show(cb);

    menu_animation_ctx_entry_t entry;

    entry.duration      = this->getShowAnimationDuration();
    entry.easing_enum   = EASING_OUT_QUAD;
    entry.subject       = &this->topOffset;
    entry.tag           = (uintptr_t) nullptr;
    entry.target_value  = 0.0f;
    entry.tick          = [this](void* userdata){ this->invalidate(); };
    entry.userdata      = nullptr;

    menu_animation_push(&entry);
}

void SelectView::draw(NVGcontext *vg, int x, int y, unsigned width, unsigned height, Style *style, FrameContext *ctx)
{
    unsigned top = this->list->getY() - style->SelectView.headerHeight - style->SelectView.listPadding;

    // Backdrop
    nvgFillColor(vg, a(ctx->theme->selectViewBackgroundColor));
    nvgBeginPath(vg);
    nvgRect(vg, x, y, width, top);
    nvgFill(vg);

    // TODO: Shadow

    // Background
    nvgFillColor(vg, a(ctx->theme->sidebarColor));
    nvgBeginPath(vg);
    nvgRect(vg, x, top, width, height - top);
    nvgFill(vg);

    // List
    this->list->frame(ctx);

    nvgFillColor(vg, a(ctx->theme->separatorColor));

    // Footer
    // TODO: Text
    nvgBeginPath(vg);
    nvgRect(vg, x + style->SettingsFrame.separatorSpacing, y + height - style->SettingsFrame.footerHeight, width - style->SettingsFrame.separatorSpacing * 2, 1);
    nvgFill(vg);

    // Header
    nvgBeginPath(vg);
    nvgRect(vg, x + style->SettingsFrame.separatorSpacing, top + style->SelectView.headerHeight - 1, width - style->SettingsFrame.separatorSpacing * 2, 1);
    nvgFill(vg);

    nvgBeginPath(vg);
    nvgFillColor(vg, a(ctx->theme->textColor));
    nvgFontFaceId(vg, ctx->fontStash->regular);
    nvgFontSize(vg, style->SelectView.headerFontSize);
    nvgTextAlign(vg, NVG_ALIGN_LEFT | NVG_ALIGN_MIDDLE);
    nvgText(vg, x + style->SelectView.headerPadding, top + style->SelectView.headerHeight / 2, this->title.c_str(), nullptr);
}

bool SelectView::onCancel()
{
    if (this->listener)
        this->listener(-1);

    Application::popView();
    return true;
}

unsigned SelectView::getShowAnimationDuration()
{
    return View::getShowAnimationDuration() / 2;
}

void SelectView::layout(NVGcontext* vg, Style *style, FontStash *stash)
{
    // Layout and move the list
    unsigned listHeight = min(SELECT_VIEW_MAX_ITEMS, this->valuesCount) * style->SelectView.listItemHeight - (unsigned) this->topOffset;
    unsigned listWidth  = style->SelectView.listWidth + style->List.marginLeftRight * 2;

    this->list->setBoundaries(
        this->width / 2 - listWidth / 2,
        this->height - style->SettingsFrame.footerHeight - listHeight - style->SelectView.listPadding + (unsigned) this->topOffset,
        listWidth,
        listHeight);
    this->list->invalidate();
}

View* SelectView::requestFocus(FocusDirection direction, View *oldFocus, bool fromUp)
{
    return this->list->requestFocus(direction, oldFocus, fromUp);
}

// TODO: Slide up animation
void SelectView::open(string title, vector<string> values, SelectListener listener, int selected)
{
    SelectView *selectView = new SelectView(title, values, listener, selected);
    Application::pushView(selectView);
}

SelectView::~SelectView()
{
    delete this->list;
}