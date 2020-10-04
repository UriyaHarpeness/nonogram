#pragma once

#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <cassert>
#include <algorithm>

#include "../logging/Logger.h"

using namespace std;

typedef pair<vector<bool>, vector<bool>> Known;
typedef pair<vector<vector<bool>>, vector<vector<bool>>> Board;

class Nonogram {
public:
    Nonogram(const string &file_name);

    ~Nonogram() = default;

    void save_board();

    const Board &get_board(int index);

    unsigned int count_known(int index);

    /**
     * I do not understand exactly how this function works, taken from my older implementation.
     */
    static unsigned int n_choose_k(unsigned int n, unsigned int k);

    static unsigned int calculate_combinations(unsigned int n, unsigned int k);

    static int get_free_space(const vector<int> &values, int size);

    static vector<bool> get_known_colored(const vector<int> &values, int size);

    void initial_optimization();

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

    static vector<bool> generate_option(const vector<int> &numbers, const vector<int> &spaces, int size);

    static vector<vector<bool>> generate_options(const Known &known, const vector<int> &numbers);

    void initialize_options();

    bool solved();

    static void filter(const Known &known, vector<vector<bool>> &options);

    static Known merge_options(Known known, vector<vector<bool>> &options);

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

    // todo: boost::dynamic_bitset may be better.
    vector<vector<vector<bool>>> m_horizontal_options, m_vertical_options;

    vector<Board> m_boards;
};
