#pragma once

#include <algorithm>
#include <cassert>
#include <set>
#include <vector>

#include "../logging/debug.h"
#include "../logging/Logger.h"

using namespace std;

/// Define Known as a pair of two vectors of bool, first one for "is set", second for the black / white values.
typedef pair<vector<bool>, vector<bool>> Known;

/// Enum for the line's direction.
enum Direction : bool {
    VERTICAL = true,
    HORIZONTAL = false,
};

class Nonogram;

/**
 * Implementation of a line, for assumptions, options generating and reducing, and resolving.
 */
class Line {
public:

    /**
     * Constructor.
     *
     * @param numbers   The numbers in the line.
     * @param size      The line's size.
     * @param direction The line's direction.
     * @param index     The line's index in the given direction.
     */
    Line(const vector<int> &numbers, unsigned int size, Direction direction, unsigned int index);

    /**
     * Destructor.
     */
    ~Line() = default;

    /**
     * N Choose K calculation.
     *
     * @param n Number of elements.
     * @param k Number of elements to choose.
     * @return  The number of combinations of choosing K elements from N.
     */
    static size_t n_choose_k(unsigned int n, unsigned int k);

    /**
     * Calculate number of combinations.
     *
     * Uses the stars and bars combinatorics equation: https://en.wikipedia.org/wiki/Stars_and_bars_(combinatorics).
     *
     * @param n Number of stars.
     * @param k Number of bars to place.
     * @return  The number of combinations of placing K bars between N stars.
     */
    static size_t calculate_combinations(unsigned int n, unsigned int k);

    /**
     * Get naive number of combinations.
     *
     * Performs naive calculation of the number of combinations, not true when a part of the line is known.
     *
     * @see Line::calculate_combinations.
     * @return  The naive number of combinations for the line.
     */
    [[nodiscard]] size_t get_combinations_number() const;

    /**
     * Get aware number of combinations.
     *
     * Performs aware calculation of the number of combinations, more correct than the naive calculation when a part of
     * the line is known.
     *
     * @param possible_per_number   The possible locations for each number in the line.
     * @return  The aware number of combinations for the line.
     */
    [[nodiscard]] inline size_t max_combinations(const vector<vector<int>> &possible_per_number) const;

    /**
     * Get the free space in the line.
     *
     * The free space of a line is the number of white elements in the line with no fixed place.
     * The calculation is as follows: Size - Sum(numbers) - (Size(numbers) - 1) = Free Space.
     * For example:
     * 1.   Size of 5 and values of 2, 2 -> free space is 0 (5 - 2 - 2 - 1 = 0).
     * 2.   Size of 6 and values of 2, 2 -> free space is 1 (6 - 2 - 2 - 1 = 1).
     * 3.   Size of 7 and values of 1, 1, 1 -> free space is 2 (7 - 1 - 1 - 1 - 2 = 2).
     *
     * @return  The free space in the line.
     */
    [[nodiscard]] unsigned int get_free_space() const;

    /**
     * Get places that must be black.
     *
     * Lines with numbers larger than their free space, have known black, because the leftmost and rightmost positioning
     * of these number have common places, the known block places are all the places in the value that are greater than
     * the free space.
     * For example:
     * 1.   Free space is 3 and value is 2 -> @@... + ...@@ = ..... , no known values.
     *      [free_space..value-1] -> [3..2] -> null.
     * 2.   Free space is 1 and value is 3 -> @@@. + .@@@ = .@@. , the places of indexes 1 and 2 are known,
     *      [free_space..value-1] -> [1..2] -> 1, 2.
     * 3.   Free space is 0 and the value is 1 -> @ + @ = @ , the place of index 1 is known,
     *      [free_space..value-1] -> [0..1] -> 0.
     *
     * @see Line::get_free_space.
     * @return  Places that must be black.
     */
    [[nodiscard]] vector<bool> get_known_colored() const;

    /**
     * Perform initial optimizations.
     *
     * Resolve values that are known without any other information about the line or generating options.
     *
     * @see Line::get_known_colored.
     * @return  Known values.
     */
    [[nodiscard]] Known initial_optimization() const;

    /**
     * Get leftmost and rightmost positions of the numbers.
     *
     * @param possible_length_by_location   The maximum number that can fit in each position of the line.
     * @return  The leftmost and rightmost positions of the numbers.
     */
    [[nodiscard]] vector<pair<int, int>> get_limits(const vector<int> &possible_length_by_location) const;

    /**
     * Expand possible positions per number.
     *
     * For example:
     * If a number of value 3 can be positioned in indexes 1 and 2, the expanded places are 1, 2, 3, and 4.
     *
     * @param possible_per_number   The possible positions of the numbers.
     * @return  The expanded possible positions of the numbers.
     */
    [[nodiscard]] vector<set<int>>
    possible_per_number_to_extended_place(const vector<vector<int>> &possible_per_number) const;

    /**
     * Contract expanded possible positions per number.
     *
     * For example:
     * If a number of value 3 can be positioned expanded in indexes 1, 2, 3 and 4, the contracted places are 1 and 2.
     *
     * @param extended_possible_per_place   The expanded possible positions of the numbers.
     * @return  The contracted possible positions of the numbers.
     */
    [[nodiscard]] vector<vector<int>>
    possible_per_extended_place_to_number(const vector<set<int>> &extended_possible_per_place) const;

    /**
     * Reduce by required with only one possible number.
     *
     * If only one number matches a known black, than it can be placed only in a matter that contains this position.
     *
     * @param extended_possible_per_place   The expanded possible positions of the numbers.
     * @param required                      Locations known to be black.
     * @param limits                        The leftmost and rightmost positions of the numbers.
     */
    void reduce_by_required(vector<set<int>> &extended_possible_per_place, const vector<int> &required,
                            const vector<pair<int, int>> &limits);

    /**
     * Reduce by blocking required.
     *
     * A number cannot be positioned in a way that conflicts with a known black, for example to start in the position
     * right after it, or to end in the position right before it, because of the required white spaces between blacks.
     *
     * @param possible_per_number   The possible positions of the numbers.
     * @param required              Locations known to be black.
     */
    void reduce_by_blocking_required(vector<vector<int>> &possible_per_number, const vector<int> &required);

    /**
     * Reduce by numbers with only one possible position.
     *
     * If a number has only one possible position, no other numbers can be present in its value and surroundings.
     *
     * @param possible_per_number   The possible positions of the numbers.
     */
    void reduce_by_single_fit(vector<vector<int>> &possible_per_number);

    /**
     * Reduce by positions which conflict with the numbers' order.
     *
     * A number cannot be positioned right to a previous number's leftmost position and its surroundings.
     *
     * @param possible_per_number   The possible positions of the numbers.
     */
    void reduce_by_order(vector<vector<int>> &possible_per_number);

    /**
     * Count number of elements in a two dimensional vector.
     *
     * @param value Two dimensional vector to count its elements.
     * @return  The number of elements.
     */
    [[nodiscard]] static unsigned int counts_elements(const vector<vector<int>> &value);

    /**
     * Generate possible positions for the numbers.
     *
     * @param known The line.
     * @return  Possible positions for the numbers, plus expanded.
     */
    pair<vector<vector<int>>, vector<set<int>>> generate_possible_per_number(const Known &known);

    /**
     * Calculate and generate known values.
     *
     * Use the functions defined above to reduce the possible positions to minimum and extract known values.
     *
     * @param known The line.
     * @return  Calculated known values.
     */
    Known calculate_known(Known known);

    /**
     * Generate an option.
     *
     * Generate an option for the line, placing the numbers in the given positions indexes.
     *
     * @param position_indexes      The indexes of the positions to place the numbers in.
     * @param possible_per_number   The possible positions of the numbers.
     * @return  The generated option.
     */
    [[nodiscard]] vector<bool>
    generate_option(const vector<int> &position_indexes, const vector<vector<int>> &possible_per_number) const;

    /**
     * Check if an option has black values where required.
     *
     * @param required  Locations known to be black.
     * @param option    The option to check.
     * @return  -1 if the option has black values where required, the index of the missing required otherwise.
     */
    inline static int check_option_by_required(const vector<int> &required, const vector<bool> &option);

    /**
     * Generate all options for the line.
     *
     * Consider the known values, and perform optimizations to generate all the possible placement options of the line.
     *
     * @param known The line.
     */
    void generate_options(const Known &known);

    /**
     * Check if an option matches the known values.
     *
     * Check if all the values that are known of the line are the same in the option.
     *
     * @param known     The line.
     * @param option    The option to check.
     * @return  Does the option match the known values.
     */
    inline static bool check_option(const Known &known, const vector<bool> &option);

    /**
     * Filter optioned by known values.
     *
     * Remove all options of the line that do not match the known values.
     *
     * @see Line::check_option.
     * @param known The line.
     */
    void filter(const Known &known);

    /**
     * Get known values of the line.
     *
     * Merge all the line's options, and positions where all the options have the same value, are known to be that
     * value.
     *
     * @param known The line.
     * @return  Known values of the line.
     */
    Known merge_options(Known known);

    friend class Nonogram;


private:

    /*
     * todo:
     * Add options to choose between bool and unsigned char in compile time (#ifdef), and if so, disable all logs
     * besides summary.
     *
     * Storing the options in vector of different types:
     * 1. bool - Takes a small space in memory, can fit entirely into cache, can make general memory accesses faster.
     * 2. unsigned char - Takes more space in memory, but specific index accessing and calculations are faster.
     *
     * The current implementation uses bool, in some cases the unsigned char can give better results.
     */
    /// The possible placements of the line.
    vector<vector<bool>> m_options;

    /// The line's numbers.
    vector<int> m_numbers;

    /// The line's size.
    unsigned int m_size;

    /// The line's direction.
    Direction m_direction;

    /// The line's index in its direction.
    unsigned int m_index;

    /// Maximum number of combinations for the line.
    size_t m_max_combinations;
};
