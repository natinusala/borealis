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

#pragma once

#include <fmt/core.h>

#include <string>

namespace brls
{

enum class LogLevel
{
    ERROR = 0,
    WARNING,
    INFO,
    DEBUG
};

class Logger
{
  public:
    static void setLogLevel(LogLevel logLevel);

    template <typename... Args>
    inline static void log(LogLevel logLevel, std::string prefix, std::string color, std::string format, Args&&... args)
    {
        if (Logger::logLevel < logLevel)
            return;

        try
        {
            fmt::print("\033{}[{}]\033[0m ", color, prefix);
            fmt::print(format, args...);
            fmt::print("\n");
        }
        catch (const std::exception& e)
        {
            // will be printed after the first fmt::print (so after the log tag)
            printf("! Invalid log format string: \"%s\": %s\n", format.c_str(), e.what());
        }

#ifdef __MINGW32__
        fflush(0);
#endif
    }

    template <typename... Args>
    inline static void error(std::string format, Args&&... args)
    {
        Logger::log(LogLevel::ERROR, "ERROR", "[0;31m", format, args...);
    }

    template <typename... Args>
    inline static void warning(std::string format, Args&&... args)
    {
        Logger::log(LogLevel::WARNING, "WARNING", "[0;33m", format, args...);
    }

    template <typename... Args>
    inline static void info(std::string format, Args&&... args)
    {
        Logger::log(LogLevel::INFO, "INFO", "[0;34m", format, args...);
    }

    template <typename... Args>
    inline static void debug(std::string format, Args&&... args)
    {
        Logger::log(LogLevel::DEBUG, "DEBUG", "[0;32m", format, args...);
    }

  private:
    inline static LogLevel logLevel = LogLevel::INFO;
};

} // namespace brls
