/*
    Copyright 2020 natinusala

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

#include <borealis/core/view.hpp>

namespace brls
{

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
     * Returns the view with the corresponding id, or nullptr
     * if it hasn't been found in the activity.
     */
    View* getView(std::string id);

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

    /**
     * Registers an action with the given parameters on the content view. The listener will be fired
     * when the user presses the key.
     *
     * The listener should return true if the action was consumed, false otherwise.
     * The sound will only be played if the listener returned true.
     *
     * A hidden action will not show up in the bottom-right hints.
     *
     * Must be called after the content view is set.
     *
     * Returns the identifier for the action, so it can be unregistered later on. Returns ACTION_NONE if the
     * action was not registered.
     */
    ActionIdentifier registerAction(std::string hintText, enum ControllerButton button, ActionListener actionListener, bool hidden = false, enum Sound sound = SOUND_NONE);

    /**
     * Unregisters an action with the given identifier on the content view.
     *
     * Must be called after the content view is set.
     */
    void unregisterAction(ActionIdentifier identifier);

    /**
     * Registers an action to exit the application with the default button BUTTON_START.
     *
     * Must be called after the content view is set.
     *
     * Returns the identifier for the action, so it can be unregistered later on. Returns ACTION_NONE if the
     * action was not registered.
     */
    ActionIdentifier registerExitAction(enum ControllerButton button = brls::BUTTON_START);

    void onWindowSizeChanged();

    View* getDefaultFocus();

    void setAlpha(float alpha);

  private:
    View* contentView = nullptr;
};

} // namespace brls
