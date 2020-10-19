#pragma once

#include <algorithm>
#include <cassert>
#include <set>
#include <vector>

#include "../logging/debug.h"
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

    [[nodiscard]] inline size_t max_combinations(const vector<vector<int>> &possible_per_number) const;

    [[nodiscard]] int get_free_space() const;

    [[nodiscard]] vector<bool> get_known_colored() const;

    [[nodiscard]] Known initial_optimization() const;

    [[nodiscard]] vector<pair<int, int>> get_limits(const vector<int> &possible_length_by_location) const;

    [[nodiscard]] vector<set<int>>
    possible_per_number_to_extended_place(const vector<vector<int>> &possible_per_number) const;

    [[nodiscard]] vector<vector<int>>
    possible_per_extended_place_to_number(const vector<set<int>> &extended_possible_per_place) const;

    void reduce_by_required(vector<set<int>> &extended_possible_per_place, const vector<int> &required,
                            const vector<pair<int, int>> &limits);

    void reduce_by_blocking_required(vector<vector<int>> &possible_per_number, const vector<int> &required);

    void reduce_by_single_fit(vector<vector<int>> &possible_per_number);

    void reduce_by_order(vector<vector<int>> &possible_per_number);

    [[nodiscard]] static int counts_elements(const vector<vector<int>> &value);

    vector<vector<int>> generate_possible_per_number(const Known &known);

    Known optimize_locations(Known known);

    [[nodiscard]] vector<bool>
    generate_option(const vector<int> &iterators, const vector<vector<int>> &possible_per_number) const;

    inline static bool check_option_by_required(const vector<int> &required, const vector<bool> &option);

    void generate_options(const Known &known);

    inline static bool check_option(const Known &known, const vector<bool> &option);

    void filter(const Known &known);

    Known merge_options(Known known);

    friend class Nonogram;

private:
    /*
     * todo: add options to choose between bool and unsigned char in compile time (#ifdef).
     * if so, disable all logs besides summary as well...
     *
     * Storing the options in vector of different types:
     * 1. bool - Takes a small space in memory, can fit entirely into cache, can make general memory accesses faster.
     * 2. unsigned char - Takes more space in memory, but specific index accessing and calculations are faster.
     *
     * The current implementation uses bool, in some cases the unsigned char can give better results.
     */
    vector<vector<bool>> m_options;

    vector<int> m_numbers;

    int m_size;

    Direction m_direction;

    int m_index;

    // Maximum combinations in case some of the line is known, if not known, the number is exact.
    size_t m_max_combinations;
};
