#pragma once

#include <algorithm>
#include <cassert>
#include <vector>

#include "../logging/Logger.h"

using namespace std;

typedef pair<vector<bool>, vector<bool>> Known;

enum Direction : bool {
    VERTICAL = true,
    HORIZONTAL = false,
};

class Nonogram;

class Line {
public:
    Line(const vector<int> &numbers, int size, Direction direction, int index);

    ~Line() = default;

    /**
     * Uses the stars and bars combinatorics equation:
     * https://en.wikipedia.org/wiki/Stars_and_bars_(combinatorics)
     */
    static size_t n_choose_k(unsigned int n, unsigned int k);

    static size_t calculate_combinations(unsigned int n, unsigned int k);

    [[nodiscard]] size_t get_combinations_number() const;

    [[nodiscard]] int get_free_space() const;

    [[nodiscard]] vector<bool> get_known_colored() const;

    [[nodiscard]] Known initial_optimization() const;

    vector<bool> generate_option(const vector<int> &spaces);

    void generate_options(const Known &known);

    static bool check_option(const Known &known, const vector<bool> &option);

    void filter(const Known &known);

    Known merge_options(Known known);

    friend class Nonogram;

private:
    vector<vector<bool>> m_options;

    vector<int> m_numbers;

    int m_size;

    Direction m_direction;

    int m_index;
};
