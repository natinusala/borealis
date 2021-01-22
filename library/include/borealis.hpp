/*
    Borealis, a Nintendo Switch UI Library
    Copyright (C) 2019-2021  natinusala
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

#pragma once

#ifndef BOREALIS_RESOURCES
#error BOREALIS_RESOURCES define missing
#endif
#define BOREALIS_ASSET(_str) BOREALIS_RESOURCES _str

// Core
#include <borealis/core/activity.hpp>
#include <borealis/core/animations.hpp>
#include <borealis/core/application.hpp>
#include <borealis/core/audio.hpp>
#include <borealis/core/event.hpp>
#include <borealis/core/i18n.hpp>
#include <borealis/core/logger.hpp>
#include <borealis/core/platform.hpp>
#include <borealis/core/repeating_task.hpp>
#include <borealis/core/style.hpp>
#include <borealis/core/theme.hpp>

//Views
#include <borealis/views/applet_frame.hpp>
#include <borealis/views/background.hpp>
#include <borealis/views/box.hpp>
#include <borealis/views/button.hpp>
#include <borealis/views/header.hpp>
#include <borealis/views/image.hpp>
#include <borealis/views/label.hpp>
#include <borealis/views/rectangle.hpp>
#include <borealis/views/scrolling_frame.hpp>
#include <borealis/views/sidebar.hpp>
#include <borealis/views/tab_frame.hpp>
#include <borealis/views/view.hpp>
