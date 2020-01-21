/*
    Borealis, a Nintendo Switch UI Library
    Copyright (C) 2019  natinusala
    Copyright (C) 2019  p-sam

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

#include <borealis/application.hpp>
#include <borealis/hint.hpp>
#include <borealis/hint_types.hpp>

#include <set>

namespace brls
{

Hint::Hint()
{
    Style* style = Application::getStyle();
    this->setHeight(style->AppletFrame.footerHeight);

    this->animateHint = true;
}

std::string getKeyIcon(Key key) {
    switch (key)
    {
        case Key::A:        return "\uE0E0";
        case Key::B:        return "\uE0E1";
        case Key::X:        return "\uE0E2";
        case Key::Y:        return "\uE0E3";
        case Key::LSTICK:   return "\uE104";
        case Key::RSTICK:   return "\uE105";
        case Key::L:        return "\uE0E4";
        case Key::R:        return "\uE0E5";
        case Key::PLUS:     return "\uE0EF";
        case Key::MINUS:    return "\uE0F0";
        case Key::DLEFT:    return "\uE0ED";
        case Key::DUP:      return "\uE0EB";
        case Key::DRIGHT:   return "\uE0EF";
        case Key::DDOWN:    return "\uE0EC";
        default:            return "\uE152";
    }
}

void Hint::draw(NVGcontext* vg, int x, int y, unsigned width, unsigned height, Style* style, FrameContext* ctx)
{
    unsigned xAdvance = x + width - style->AppletFrame.separatorSpacing - style->AppletFrame.footerTextSpacing;

    y      = y + height - style->AppletFrame.footerHeight;
    height = style->AppletFrame.footerHeight;

    unsigned middle = y + height / 2;

    nvgTextAlign(vg, NVG_ALIGN_RIGHT | NVG_ALIGN_MIDDLE);

    float bounds[4];
    View *parent = Application::currentFocus;
    std::set<Key> hintsDrawn;

    while (parent != nullptr) {

        for (auto &[key, keyHint] : parent->keyHints)
        {
            if (keyHint.hidden)
                continue;
            
            if (hintsDrawn.find(key) != hintsDrawn.end())
                continue;

            hintsDrawn.insert(key);

            if (this->animateHint) {
                if (keyHint.available)
                    nvgFillColor(vg, a(ctx->theme->textColor));
                else
                    nvgFillColor(vg, a(ctx->theme->descriptionColor));
            }
            else {
                if (keyHint.available)
                    nvgFillColor(vg, ctx->theme->textColor);
                else
                    nvgFillColor(vg, ctx->theme->descriptionColor);
            }

            nvgBeginPath(vg);
            nvgFontSize(vg, style->AppletFrame.footerTextSize);
            nvgText(vg, xAdvance, middle, keyHint.hintText.c_str(), nullptr);
            nvgTextBounds(vg, xAdvance, middle, keyHint.hintText.c_str(), nullptr, bounds);

            xAdvance -= style->AppletFrame.footerTextSpacing / 3;
            xAdvance -= (unsigned)(bounds[2] - bounds[0]);

            nvgBeginPath(vg);
            nvgFontSize(vg, style->AppletFrame.footerHintSize);
            nvgText(vg, xAdvance, middle, getKeyIcon(key).c_str(), nullptr);
            nvgTextBounds(vg, xAdvance, middle, getKeyIcon(key).c_str(), nullptr, bounds);

            xAdvance -= (unsigned)(bounds[2] - bounds[0]);
            xAdvance -= style->AppletFrame.footerTextSpacing + style->AppletFrame.separatorSpacing / 2;
        }

        parent = parent->getParent();
    }

}

void Hint::handleInput(char button)
{   
    View *hintParent = Application::currentFocus;

    std::set<Key> inputConsumed;

    while (hintParent != nullptr)
    {
        for (auto &[key, keyHint] : hintParent->keyHints)
        {
            if (key != static_cast<Key>(button))
                continue;

            if (keyHint.available)
                if (inputConsumed.find(key) == inputConsumed.end())
                    if (keyHint.buttonListener())
                        inputConsumed.insert(key);
        }

        hintParent = hintParent->getParent();
    }
}

} // namespace brls
