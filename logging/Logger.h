#pragma once

#include <chrono>
#include <map>
#include <sstream>
#include <algorithm>

using namespace std;

// Logger, version 0.1: a simple logging class
// Taken and refactored from Petru Marginean's article in Dr. Dobbs - https://www.drdobbs.com/cpp/logging-in-c/201804215
enum LogLevel {
    ERROR = 5,
    WARNING = 4,
    INFO = 3,
    DEBUG = 2,
    TRACE = 1
};

class Logger {
public:
    static const map<LogLevel, string> level_strings;

    Logger(LogLevel level = INFO);

    virtual ~Logger();

    void flush();

    static string zfill(string str, int length, char filler = '0');

    static string pad(string str, int length, char filler = ' ');

    static LogLevel get_level_by_string(const string &level);

    void init_line();

    ostringstream &Get();

    static LogLevel enabled_level;

private:
    LogLevel m_level;

    ostringstream os;

    bool m_empty;
};
