/*
    Borealis, a Nintendo Switch UI Library
    Copyright (C) 2020  WerWolv

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

#include <borealis/actions.hpp>
#include <borealis/application.hpp>
#include <borealis/hint.hpp>
#include <set>

namespace brls
{

Hint::Hint(bool animate)
    : animate(animate)
{
    Style* style = Application::getStyle();
    this->setHeight(style->AppletFrame.footerHeight);

    // Subscribe to all events
    this->globalFocusEventSubscriptor = Application::getGlobalFocusChangeEvent()->subscribe([this](View* newFocus) {
        this->invalidate();
    });

    this->globalHintsUpdateEventSubscriptor = Application::getGlobalHintsUpdateEvent()->subscribe([this]() {
        this->invalidate();
    });
}

Hint::~Hint()
{
    // Unregister all events
    Application::getGlobalFocusChangeEvent()->unsubscribe(this->globalFocusEventSubscriptor);
    Application::getGlobalHintsUpdateEvent()->unsubscribe(this->globalHintsUpdateEventSubscriptor);
}

std::string getKeyIcon(Key key)
{
    switch (key)
    {
        case Key::A:
            return "\uE0E0";
        case Key::B:
            return "\uE0E1";
        case Key::X:
            return "\uE0E2";
        case Key::Y:
            return "\uE0E3";
        case Key::LSTICK:
            return "\uE104";
        case Key::RSTICK:
            return "\uE105";
        case Key::L:
            return "\uE0E4";
        case Key::R:
            return "\uE0E5";
        case Key::PLUS:
            return "\uE0EF";
        case Key::MINUS:
            return "\uE0F0";
        case Key::DLEFT:
            return "\uE0ED";
        case Key::DUP:
            return "\uE0EB";
        case Key::DRIGHT:
            return "\uE0EF";
        case Key::DDOWN:
            return "\uE0EC";
        default:
            return "\uE152";
    }
}

void Hint::draw(NVGcontext* vg, int x, int y, unsigned width, unsigned height, Style* style, FrameContext* ctx)
{
    nvgTextAlign(vg, NVG_ALIGN_RIGHT | NVG_ALIGN_MIDDLE);

    for (unsigned i = 0; i < this->buttonHints.size(); i++)
    {
        if (this->hintAvailables[i])
            nvgFillColor(vg, animate ? a(ctx->theme->textColor) : ctx->theme->textColor);
        else
            nvgFillColor(vg, animate ? a(ctx->theme->descriptionColor) : ctx->theme->descriptionColor);

        nvgBeginPath(vg);
        nvgFontSize(vg, style->AppletFrame.footerTextSize);
        nvgText(vg, this->hintXPositions[i], this->hintYPosition, this->buttonHints[i].c_str(), nullptr);
    }
}

void Hint::layout(NVGcontext* vg, Style* style, FontStash* stash)
{
    unsigned x        = this->x + this->width - style->AppletFrame.separatorSpacing - style->AppletFrame.footerTextSpacing;
    unsigned xAdvance = x + style->AppletFrame.separatorSpacing / 2;

    unsigned y      = this->y + this->height - style->AppletFrame.footerHeight;
    unsigned height = style->AppletFrame.footerHeight;

    unsigned middle = y + height / 2;

    this->hintYPosition = middle;

    float bounds[4];
    std::string hintText;

    // Check if the focused element is still a child of the same parent as the hint view's
    {
        View* focusParent    = Application::currentFocus;
        View* hintBaseParent = this;

        while (focusParent != nullptr)
        {
            if (focusParent->getParent() == nullptr)
                break;
            focusParent = focusParent->getParent();
        }

        while (hintBaseParent != nullptr)
        {
            if (hintBaseParent->getParent() == nullptr)
                break;
            hintBaseParent = hintBaseParent->getParent();
        }

        if (focusParent != hintBaseParent)
            return;
    }

    this->hintCount = 2;
    this->buttonHints.clear();
    this->hintAvailables.clear();
    this->hintXPositions.clear();

    // Reserve space for A, B and (+) hint
    this->buttonHints.push_back(" ");
    this->buttonHints.push_back(" ");
    this->buttonHints.push_back(" ");
    this->hintAvailables.push_back(false);
    this->hintAvailables.push_back(false);
    this->hintAvailables.push_back(false);
    this->hintXPositions.push_back(0);
    this->hintXPositions.push_back(0);
    this->hintXPositions.push_back(0);

    // We only ever want one action per key
    std::set<Key> addedKeys;

    View* focusParent = Application::currentFocus;

    while (focusParent != nullptr)
    {
        for (auto& action : focusParent->actions)
        {
            if (action.hidden)
                continue;

            if (addedKeys.find(action.key) != addedKeys.end())
                continue;

            addedKeys.insert(action.key);

            hintText = getKeyIcon(action.key) + "  " + action.hintText;
            hintCount++;

            nvgSave(Application::getNVGContext());
            nvgFontSize(vg, style->AppletFrame.footerTextSize);
            nvgTextBounds(vg, x, middle, hintText.c_str(), nullptr, bounds);
            nvgRestore(Application::getNVGContext());

            unsigned hintWidth = (unsigned)(bounds[2] - bounds[0]) + style->AppletFrame.footerTextSpacing + style->AppletFrame.separatorSpacing / 2;

            if (action.key == Key::A)
            {
                this->buttonHints[0]    = hintText;
                this->hintAvailables[0] = action.available;
                this->hintXPositions[0] = x;

                for (unsigned i = 3; i < this->hintCount; i++)
                    this->hintXPositions[i] -= hintWidth;
            }
            else if (action.key == Key::B)
            {
                this->buttonHints[1]    = hintText;
                this->hintAvailables[1] = action.available;
                this->hintXPositions[1] = this->hintXPositions[0] - hintWidth + style->AppletFrame.footerTextSpacing;

                for (unsigned i = 3; i < this->hintCount; i++)
                    this->hintXPositions[i] -= hintWidth;
            }
            else if (action.key == Key::PLUS)
            {
                this->buttonHints[2]    = hintText;
                this->hintAvailables[2] = action.available;
                this->hintXPositions[2] = this->hintXPositions[1] - hintWidth - style->AppletFrame.footerTextSpacing / 2;

                for (unsigned i = 3; i < this->hintCount; i++)
                    this->hintXPositions[i] -= hintWidth;
            }
            else
            {
                this->buttonHints.push_back(hintText);
                this->hintAvailables.push_back(action.available);
                this->hintXPositions.push_back(xAdvance);
            }

            xAdvance -= hintWidth;
        }

        focusParent = focusParent->getParent();
    }
}

void Hint::willAppear()
{
    // Push ourself to hide other hints
    // We assume that we are the top-most hint
    Hint::pushHint(this);
}

void Hint::willDisappear()
{
    // Pop ourself to show other hints
    Hint::popHint(this);
}

void Hint::pushHint(Hint* hint)
{
    // If the hint is already in the stack, remove it and put it back on top
    Hint::globalHintStack.erase(std::remove(Hint::globalHintStack.begin(), Hint::globalHintStack.end(), hint), Hint::globalHintStack.end());
    Hint::globalHintStack.push_back(hint);

    //Trigger animation
    Hint::animateHints();
}

void Hint::popHint(Hint* hint)
{
    Hint::globalHintStack.erase(std::remove(Hint::globalHintStack.begin(), Hint::globalHintStack.end(), hint), Hint::globalHintStack.end());
    Hint::animateHints();

    // animateHints() won't call hide() on the hint since it's been removed from the stack
    // but it doesn't matter since it is getting destroyed anyay
}

void Hint::animateHints()
{
    for (size_t i = 0; i < Hint::globalHintStack.size(); i++)
    {
        // Last one = top one: show
        if (i == Hint::globalHintStack.size() - 1)
            Hint::globalHintStack[i]->show([](){}, false);
        else
            Hint::globalHintStack[i]->hide([](){}, false);
    }
}

void Hint::handleInput(char button)
{
    View* hintParent = Application::currentFocus;

    while (hintParent != nullptr)
    {
        for (auto& action : hintParent->actions)
        {
            if (action.key != static_cast<Key>(button))
                continue;

            if (action.available)
                action.actionListener();
        }

        hintParent = hintParent->getParent();
    }
}

} // namespace brls
