/*
    Borealis, a Nintendo Switch UI Library
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

#include <borealis/core/activity.hpp>
#include <borealis/core/application.hpp>

namespace brls
{

Activity::Activity()
{
}

void Activity::setContentView(View* view)
{
    if (this->contentView)
    {
        this->contentView->willDisappear();
        delete this->contentView;
    }

    this->contentView = view;
    // willAppear is only called when the activity is pushed onto the stack

    this->resizeToFitWindow();
}

void Activity::resizeToFitWindow()
{
    if (!this->contentView)
        return;

    this->contentView->setDimensions(Application::contentWidth, Application::contentHeight);
}

View* Activity::createContentView()
{
    return nullptr;
}

float Activity::getShowAnimationDuration(TransitionAnimation animation)
{
    Style style = Application::getStyle();

    if (animation == TransitionAnimation::SLIDE_LEFT || animation == TransitionAnimation::SLIDE_RIGHT)
        return style["brls/animations/show_slide"];

    return style["brls/animations/show"];
}

void Activity::onWindowSizeChanged()
{
    this->resizeToFitWindow();
}

bool Activity::isTranslucent()
{
    if (!this->contentView)
        return true;

    return this->contentView->isTranslucent();
}

void Activity::willAppear(bool resetState)
{
    if (this->contentView)
        this->contentView->willAppear(resetState);
}

void Activity::willDisappear(bool resetState)
{
    if (this->contentView)
        this->contentView->willDisappear(resetState);
}

void Activity::setInFadeAnimation(bool inFadeAnimation)
{
    if (this->contentView)
        this->contentView->setInFadeAnimation(inFadeAnimation);
}

void Activity::show(std::function<void(void)> cb, bool animate, float animationDuration)
{
    if (this->contentView)
        this->contentView->show(cb, animate, animationDuration);
}

void Activity::hide(std::function<void(void)> cb, bool animate, float animationDuration)
{
    if (this->contentView)
        this->contentView->hide(cb, animate, animationDuration);
}

bool Activity::isHidden()
{
    if (!this->contentView)
        return false;

    return this->contentView->isHidden();
}

void Activity::registerAction(std::string hintText, enum ControllerButton button, ActionListener actionListener, bool hidden, enum Sound sound)
{
    if (this->contentView)
        this->contentView->registerAction(hintText, button, actionListener, hidden, sound);
}

View* Activity::getDefaultFocus()
{
    if (!this->contentView)
        return nullptr;

    return this->contentView->getDefaultFocus();
}

void Activity::setAlpha(float alpha)
{
    if (this->contentView)
        this->contentView->setAlpha(alpha);
}

View* Activity::getContentView()
{
    return this->contentView;
}

View* Activity::getView(std::string id)
{
    if (!this->contentView)
        return nullptr;

    return this->contentView->getView(id);
}

Activity::~Activity()
{
    if (this->contentView)
    {
        this->contentView->willDisappear();

        delete this->contentView;
        this->contentView = nullptr;
    }
}

} // namespace brls
