#pragma once

#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <cassert>
#include <algorithm>

#include "../logging/Logger.h"
#include "../line/Line.h"

using namespace std;

typedef pair<vector<vector<bool>>, vector<vector<bool>>> Board;

class Nonogram {
public:
    Nonogram(const string &file_name);

    ~Nonogram() = default;

    void save_board();

    const Board &get_board(int index);

    unsigned int count_known(int index);

    void print_board(LogLevel level = INFO, const string &black = color_black, const string &white = color_white,
                     const string &unknown = color_unknown);

    Known get_horizontal(int y);

    Known get_vertical(int x);

    Known get_board_horizontal(int y, const Board &board);

    Known get_board_vertical(int x, const Board &board);

    void set_horizontal(const Known &known, int y);

    void set_vertical(const Known &known, int x);

    bool horizontal_changed(int y, int first = -2, int second = -1);

    bool vertical_changed(int x, int first = -2, int second = -1);

    void initial_optimization();

    void initialize_options();

    bool solved();

    void step();

    void solve();

    void save(const string &file_name);

    static const string color_black;
    static const string color_white;
    static const string color_empty;
    static const string color_unknown;

private:
    int m_x, m_y;

    vector<vector<int>> m_horizontal, m_vertical;

    vector<vector<bool>> m_value, m_set;

    // todo:
    /*
     * Storing the options in vector of different types:
     * 1. bool - Takes a small space in memory, can fit entirely into cache, can make general memory accesses faster.
     * 2. unsigned char - Takes more space in memory, but specific index accessing and calculations are faster.
     *
     * Summary:
     * Until the partial options generation is implemented, bool is better, since there's avery real limit for memory,
     * after this is implemented, and not so much memory will be used, this may help speed up calculations and accesses.
     */
    vector<Line> m_lines;

    vector<Board> m_boards;

    int m_referred_number;
};
