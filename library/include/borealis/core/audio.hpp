/*
    Copyright 2021 natinusala

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

namespace brls
{

// TODO: expose internal stuff to allow using pulsar directly

enum Sound
{
    SOUND_NONE = 0, // no sound

    SOUND_FOCUS_CHANGE, // played when the focus changes
    SOUND_FOCUS_ERROR, // played when the user wants to go somewhere impossible (while the highlight wiggles)
    SOUND_CLICK, // played when the click action runs
    SOUND_FOCUS_SIDEBAR, // played when the focus changes to a sidebar item
    SOUND_CLICK_ERROR, // played when the user clicks a disabled button / a view focused with no click action
    SOUND_HONK, // honk
    SOUND_CLICK_SIDEBAR, // played when a sidebar item is clicked
    SOUND_TOUCH_UNFOCUS, // played when touch focus has been interrupted
    SOUND_TOUCH, // played when touch not requires it's own click sound

    _SOUND_MAX, // not an actual sound, just used to count of many sounds there are
};

// Platform agnostic Audio player
// Each platform's AudioPlayer is responsible for managing the enum Sound -> internal representation map
class AudioPlayer
{
  public:
    virtual ~AudioPlayer() {};

    /**
     * Preemptively loads the given sound so that it's ready to be played
     * when needed.
     *
     * Returns a boolean indicating if the sound has been loaded or not.
     */
    virtual bool load(enum Sound sound) = 0;

    /**
     * Plays the given sound.
     *
     * The AudioPlayer should not assume that the sound has been
     * loaded already, and must load it if needed.
     *
     * Returns a boolean indicating if the sound has been played or not.
     */
    virtual bool play(enum Sound sound, float pitch = 1) = 0;
};

// An AudioPlayer that does nothing
class NullAudioPlayer : public AudioPlayer
{
  public:
    bool load(enum Sound sound) override
    {
        return false;
    }

    bool play(enum Sound sound, float pitch) override
    {
        return false;
    }
};

} // namespace brls
