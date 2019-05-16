#pragma once

// Useful macros
#ifndef __SWITCH__
#define ASSET(_str) "./resources/" _str
#else
#define ASSET(_str) "romfs:/" _str
#endif

// Library
#include <Application.hpp>