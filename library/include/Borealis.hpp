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

#pragma once

// Useful macros
#ifndef __SWITCH__
#define ASSET(_str) "./resources/" _str
#else
#define ASSET(_str) "romfs:/" _str
#endif

// Library
#include <Application.hpp>
#include <View.hpp>
#include <AppletFrame.hpp>
#include <TabFrame.hpp>
#include <Rectangle.hpp>
#include <BoxLayout.hpp>
#include <Theme.hpp>
#include <Sidebar.hpp>
#include <Style.hpp>
#include <List.hpp>
#include <Label.hpp>
#include <CrashFrame.hpp>
#include <Button.hpp>
#include <Table.hpp>
#include <Dropdown.hpp>
#include <Logger.hpp>
#include <StagedAppletFrame.hpp>
#include <ProgressDisplay.hpp>
#include <ProgressSpinner.hpp>
#include <Image.hpp>
#include <RepeatingTask.hpp>
