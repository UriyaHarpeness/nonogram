#include "Logger.h"

LogLevel Logger::enabled_level;

const map<LogLevel, string> Logger::level_strings = {
        {ERROR,   "ERROR"},
        {WARNING, "WARNING"},
        {INFO,    "INFO"},
        {DEBUG,   "DEBUG"},
        {TRACE,   "TRACE"},
};

Logger::Logger(LogLevel level) : m_level(level), m_empty(true) {}

string Logger::zfill(string str, unsigned int length, char filler) {
    if (str.length() < length) {
        str.insert(0, string(length - str.length(), filler));
    }
    return str;
}

string Logger::pad(string str, unsigned int length, char filler) {
    if (str.length() < length) {
        str.resize(length, filler);
    }
    return str;
}

LogLevel Logger::get_level_by_string(const string &level) {
    return find_if(Logger::level_strings.begin(), Logger::level_strings.end(), [&](const pair<LogLevel, string> &pair) {
        return pair.second == level;
    })->first;
}

void Logger::init_line() {
    time_t now_time = time(nullptr);
    tm *now = localtime(&now_time);
    os << "[" << 1900 + now->tm_year << "/" << zfill(to_string(1 + now->tm_mon), 2) << "/"
       << zfill(to_string(now->tm_mday), 2) << " ";
    os << zfill(to_string(now->tm_hour), 2) << ":" << zfill(to_string(now->tm_min), 2) << ":"
       << zfill(to_string(now->tm_sec), 2) << ".";
    os << zfill(to_string(
            (chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count() %
             1000) / 10), 2);
    os << "] " << pad(level_strings.at(m_level) + ": ", 9, ' ');

    m_empty = false;
}

ostringstream &Logger::get() {
    init_line();

    return os;
}

Logger::~Logger() {
    flush();
}

void Logger::flush() {
    if ((m_level >= enabled_level) && !m_empty) {
        fprintf(stderr, "%s", os.str().c_str());
        fflush(stderr);
        os.str("");
        os.clear();
        os.seekp(0);
        m_empty = true;
    }
}
