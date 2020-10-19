#pragma once

#include <chrono>
#include <map>
#include <sstream>
#include <algorithm>

using namespace std;

/**
 * Logger, version 0.1: a simple logging class.
 * Taken and refactored from Petru Marginean's article in Dr. Dobbs - https://www.drdobbs.com/cpp/logging-in-c/201804215
 */

/// The available log levels.
enum LogLevel {
    ERROR = 5,
    WARNING = 4,
    INFO = 3,
    DEBUG = 2,
    TRACE = 1
};

/**
 * Implementation of a logger, supports different levels and reuse.
 */
class Logger {
public:

    /**
     * Constructor.
     *
     * @param level The level to use for logging.
     */
    explicit Logger(LogLevel level = INFO);

    /**
     * Destructor.
     *
     * Flush the values in the stream if it is not empty, @see Logger::flush.
     */
    virtual ~Logger();

    /**
     * Flush the values in the stream if it is not empty.
     */
    void flush();

    /**
     * Right padding.
     *
     * @param str       The string to pad.
     * @param length    The desired final length.
     * @param filler    The character used to fill up to the desired length.
     * @return  The padded string.
     */
    static string zfill(string str, unsigned int length, char filler = '0');

    /**
     * Left padding.
     *
     * @param str       The string to pad.
     * @param length    The desired final length.
     * @param filler    The character used to fill up to the desired length.
     * @return  The padded string.
     */
    static string pad(string str, unsigned int length, char filler = ' ');

    /**
     * Get log level by string.
     *
     * @param level The log level string representation.
     * @return  The matching log level enum value.
     */
    static LogLevel get_level_by_string(const string &level);

    /**
     * Initialize a stream.
     *
     * Adds to the stream the timestamp and the log level, preparing for actual data writing.
     */
    void init_line();

    /**
     * Get the logger's stream.
     *
     * Initializes the stream with `Line::init_line`.
     *
     * @return  The logger's stream.
     */
    ostringstream &get();

    /// The lowest enabled log level, logs with levels below this value are dropped.
    static LogLevel enabled_level;

    /// Log level enum values and string representation mapping.
    static const map<LogLevel, string> level_strings;


private:

    /// The log level.
    LogLevel m_level;

    /// The log stream.
    ostringstream os;

    /// Is the stream empty.
    bool m_empty;
};
