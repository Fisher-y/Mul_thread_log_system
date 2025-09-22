#pragma once
#include <string>
enum class LogLevel { TRACE, DEBUG, INFO, WARNING, ERROR, FATAL };
std::string levelToString(LogLevel level);