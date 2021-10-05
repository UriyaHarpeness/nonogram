#include <iostream>

#include "logging/debug.h"
#include "nonogram/Nonogram.h"

using namespace std;

int main(int argc, char *argv[]) {
    Logger::enabled_level = Logger::get_level_by_string("TRACE");

    string str = "?????.????@??.?????";
    Known known;
    for (const auto c : str) {
        known.first.push_back(c != '?');
        known.second.push_back(c == '@');
    }
    Line l({3, 4, 1}, str.length(), HORIZONTAL, 1);
    auto o = l.calculate_known(known);

    Logger(DEBUG).get() << str << endl;
    Logger logger(DEBUG);
    auto &output = logger.get();
    for (int i = 0; i < o.first.size(); i++) {
        output << (o.first[i] ? (o.second[i] ? '@' : '.') : '?');
    }
    output << endl;
    logger.flush();

    return 0;
}
