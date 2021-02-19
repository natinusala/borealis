/*
    Copyright 2019 natinusala
    Copyright 2019 p-sam

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
