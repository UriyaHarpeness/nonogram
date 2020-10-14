#include <iostream>

#include "logging/debug.h"
#include "nonogram/Nonogram.h"

using namespace std;

int main(int argc, char *argv[]) {
    // Setup configurations.
    string file_name;
    string log_level = "INFO";
    string output = "solution.txt";
    for (int i = 1; i < argc; i += 2) {
        if (string(argv[i]) == "--log-level") {
            log_level = argv[i + 1];
        }
        if (string(argv[i]) == "--input") {
            file_name = argv[i + 1];
        }
        if (string(argv[i]) == "--output") {
            output = argv[i + 1];
        }
    }

    if (file_name.empty() || log_level.empty()) {
        throw runtime_error(
                "Usage: nonogram --input [FILE_PATH] [--log-level [LOG_LEVEL TRACE|DEBUG|INFO|WARNING|ERROR]] [--output FILE_PATH]");
    }

    Logger::enabled_level = Logger::get_level_by_string(log_level);

    // Load the nonogram.
    Nonogram nonogram(file_name);

#if DEBUG_LOGS
    Logger(INFO).Get() << "Empty Board" << endl;
    nonogram.print_board();
#endif // DEBUG_LOGS

    // Solve the nonogram.
    nonogram.solve();

#if DEBUG_SUMMARY
    Logger(ERROR).Get() << endl;
    Logger(ERROR).Get() << "Solution" << endl;
    nonogram.print_board(ERROR, Nonogram::color_black, Nonogram::color_empty, Nonogram::color_unknown);
#endif // DEBUG_SUMMARY

    // Save the nonogram.
    nonogram.save(output);

    return 0;
}
