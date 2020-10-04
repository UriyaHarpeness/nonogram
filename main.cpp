#include <iostream>

#include "nonorgram/Nonogram.h"
#include "logging/Logger.h"

using namespace std;

int main(int argc, char *argv[]) {
    // Setup configurations
    string file_name;
    string log_level = "INFO";
    for (int i = 1; i < argc; i += 2) {
        if (string(argv[i]) == "--log-level") {
            log_level = argv[i + 1];
        }
        if (string(argv[i]) == "--name") {
            file_name = argv[i + 1];
        }
    }

    if (file_name.empty() || log_level.empty()) {
        throw runtime_error(
                "Usage: nonogram --name [FILE_PATH] [--log-level [LOG_LEVEL TRACE|DEBUG|INFO|WARNING|ERROR]]");
    }

    Logger::enabled_level = Logger::get_level_by_string(log_level);

    // Load the nonogram
    Nonogram nonogram(file_name);

    Logger(INFO).Get() << "Empty Board" << endl;
    nonogram.print_board();

    // Solve the nonogram
    nonogram.solve();

    Logger(INFO).Get() << endl;
    Logger(ERROR).Get() << "Solution" << endl;
    nonogram.print_board(ERROR, Nonogram::color_black, Nonogram::color_empty, Nonogram::color_unknown);

    // Save the nonogram
    nonogram.save("solution.txt");

    return 0;
}
