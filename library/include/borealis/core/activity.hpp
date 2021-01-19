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

#pragma once

#include <borealis/views/view.hpp>

namespace brls
{

// TODO: once things are back to normal and we can test it, remove the focus stack from Application and put the focus in each activity instead to have only one stack

#define CONTENT_FROM_XML_RES(x) \
    brls::View* createContentView() override { return brls::View::createFromXMLResource(x); }
#define CONTENT_FROM_XML_FILE(x) \
    brls::View* createContentView() override { return brls::View::createFromXMLFile(x); }
#define CONTENT_FROM_XML_STR(x) \
    brls::View* createContentView() override { return brls::View::createFromXMLString(x); }

// An activity is a "screen" of your app in which the library adds
// the UI components. The app is made of a stack of activities, each activity
// containing a views tree.
class Activity
{
  public:
    Activity();
    virtual ~Activity();

    /**
     * Sets the content view of this activity, aka
     * the root view of the tree.
     *
     * When the activity is pushed, setContentView() is
     * automatically called with the result of createContentView().
     * As such, you should override createContentView() if you want
     * to use XML in your activity.
     */
    void setContentView(View* view);

    /**
     * Called when the activity is created. Should return the activity content view, if any.
     *
     * Returning nullptr means the content is not known when the activity is created.
     *
     * You can use View::createFromXMLFile(), View::createFromXMLResource() and View::createFromXMLString() to load
     * a view from respectively an XML file path, an XML name in the resources/xml romfs directory and an XML string.
     *
     * The CONTENT_FROM_XML_FILE, CONTENT_FROM_XML_RES, CONTENT_FROM_XML_STR macros
     * are made to make this process simpler: just use them in the public block of your activity
     * header and it will override createContentView() with the right code for you.
     *
     * The onContentAvailable() method will be called once the content has been created, so that
     * you can get the references to the activity views (by id).
     */
    virtual View* createContentView();

    /**
     * Called when the content view is created, so that
     * you can get the references to the activity views (by id).
     */
    virtual void onContentAvailable() {};

    View* getContentView();

    /**
     * Resizes the activity to fit the window. Called when the activity
     * is created and when the window is resized (Switch dock counts as window resize).
     */
    void resizeToFitWindow();

    /**
     * Returns the duration of the activity show / hide animation.
     */
    virtual float getShowAnimationDuration(TransitionAnimation animation);

    /**
     * Is this activity translucent, aka can we see the
     * activities under it in the stack?
     */
    bool isTranslucent();

    void willAppear(bool resetState = false);
    void willDisappear(bool resetState = false);

    /**
     * If set to true, will force the activity to be translucent.
     */
    void setInFadeAnimation(bool translucent);

    /**
      * Shows the activity with a fade in animation, or no animation at all.
      */
    void show(std::function<void(void)> cb, bool animate, float animationDuration);

    /**
      * Hides the activity with a fade in animation, or no animation at all.
      */
    void hide(std::function<void(void)> cb, bool animate, float animationDuration);

    bool isHidden();

    void registerAction(std::string hintText, Key key, ActionListener actionListener, bool hidden = false, enum Sound sound = SOUND_NONE);

    void onWindowSizeChanged();

    View* getDefaultFocus();

    void setAlpha(float alpha);

  protected:
    /**
     * Returns the view with the corresponding id, or nullptr
     * if it hasn't been found in the activity.
     */
    View* getView(std::string id);

  private:
    View* contentView = nullptr;
};

} // namespace brls
