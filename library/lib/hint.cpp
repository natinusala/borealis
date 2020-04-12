/*
    Borealis, a Nintendo Switch UI Library
    Copyright (C) 2020  WerWolv
    Copyright (C) 2020  natinusala

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

namespace brls
{

Hint::Hint(bool animate) :
    BoxLayout(BoxLayoutOrientation::HORIZONTAL),
    animate(animate)
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
            Hint::globalHintStack[i]->show([]() {}, false);
        else
            Hint::globalHintStack[i]->hide([]() {}, false);
    }
}

} // namespace brls
