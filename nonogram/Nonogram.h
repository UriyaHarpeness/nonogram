#pragma once

#include <vector>
#include <fstream>
#include <sstream>
#include <cassert>
#include <algorithm>

#include "../logging/Logger.h"
#include "../line/Line.h"

using namespace std;

/// Define Board as a pair of two dimensional vectors of bool, first one for "is set", second for the black / white
/// values.
typedef pair<vector<vector<bool>>, vector<vector<bool>>> Board;

/**
 * Implementation of a nonogram, for loading, solving, and saving.
 */
class Nonogram {
public:

    /**
     * Constructor.
     *
     * Load the nonogram file, create data required for solving, like the Lines, save the board metadata and the board
     * itself.
     *
     * @param file_name The file to load the nonogram definition from.
     */
    explicit Nonogram(const string &file_name);

    /**
     * Destructor.
     */
    ~Nonogram() = default;

    /**
     * Save the current board state.
     *
     * Adds the current values of `m_set` and `m_value` to the list of board states.
     */
    void save_board();

    /**
     * Get a board.
     *
     * @param index The index of the board, if below 0, takes that board from the end + 1, for example, -1 is the last
     *              board.
     * @return  The board at the requested index.
     */
    [[nodiscard]] const Board &get_board(int index) const;

    /**
     * Count how many values are known in a board.
     *
     * Used to help keep track of solving, can tell if another line is need to be resolved, or if the board is
     * unsolvable.
     *
     * @param index The index of the board to count known for, @see Nonogram::get_board.
     * @return  The known count for the board.
     */
    [[nodiscard]] unsigned int count_known(int index) const;

    /**
     * Print the current board state.
     *
     * @param level     Log level to use for printing.
     * @param black     Representation for the known black values.
     * @param white     Representation for the known white values.
     * @param unknown   Representation for the unknown values.
     */
    void print_board(LogLevel level = INFO, const string &black = color_black, const string &white = color_white,
                     const string &unknown = color_unknown) const;

    /**
     * Get horizontal from a board.
     *
     * @param y     The position of the horizontal line.
     * @param board The board to get the line from.
     * @return  The horizontal line.
     */
    [[nodiscard]] Known get_board_horizontal(unsigned int y, const Board &board) const;

    /**
     * Get vertical from a board.
     *
     * @param x     The position of the vertical line.
     * @param board The board to get the line from.
     * @return  The vertical line.
     */
    [[nodiscard]] Known get_board_vertical(unsigned int x, const Board &board) const;

    /**
     * Get horizontal line.
     *
     * @param y The position of the horizontal line.
     * @return  The horizontal line.
     */
    [[nodiscard]] Known get_horizontal(unsigned int y) const;

    /**
     * Get vertical line.
     *
     * @param x The position of the vertical line.
     * @return  The vertical line.
     */
    [[nodiscard]] Known get_vertical(unsigned int x) const;

    /**
     * Set horizontal line.
     *
     * Add items known from the line to the board.
     *
     * @param known The line.
     * @param y     The position of the line.
     */
    void set_horizontal(const Known &known, unsigned int y);

    /**
     * Set vertical line.
     *
     * Add items known from the line to the board.
     *
     * @param known The line.
     * @param x     The position of the line.
     */
    void set_vertical(const Known &known, unsigned int x);

    /**
     * Check for horizontal line change.
     *
     * Check if the line changed between the two board state.
     *
     * @param y         The position of the line.
     * @param first     The index of the first board, @see Nonogram::get_board.
     * @param second    The index of the second board, @see Nonogram::get_board.
     * @return  Is the line changed.
     */
    [[nodiscard]] bool horizontal_changed(unsigned int y, int first = -2, int second = -1) const;

    /**
     * Check for vertical line change.
     *
     * Check if the line changed between the two board state.
     *
     * @param x         The position of the line.
     * @param first     The index of the first board, @see Nonogram::get_board.
     * @param second    The index of the second board, @see Nonogram::get_board.
     * @return  Is the line changed.
     */
    [[nodiscard]] bool vertical_changed(unsigned int x, int first = -2, int second = -1) const;

    /**
     * Perform initial optimization.
     *
     * See `Line::initial_optimization`s documentation.
     */
    void initial_optimization();

    /**
     * Start referring to a new line.
     *
     * Sort the lines that are not referred by the maximum number of options, and add the line with the lowest number.
     * Since generating and filtering the options are the actions that take most of the time, the sorting it very
     * important.
     */
    void add_line();

    /**
     * Check if the nonogram is solved.
     *
     * @return  Is the nonogram solved.
     */
    bool solved();

    /**
     * Perform a single step of solving.
     *
     * A step is comprised of three parts:
     * 1.   Perform optimization and assumptions on lines that are not referred to extract known.
     * 2.   Filter the referred lines by what is known on the line in the board.
     * 3.   Merge the referred lines' options in order to extract known for the lines.
     */
    void step();

    /**
     * Solve the nonogram.
     *
     * The following steps are done to solve the nonogram:
     * 1.   Call `Nonogram::initial_optimization`.
     * 2.   Calling `Nonogram::step` in a loop until the known count (@see Nonogram::count_known) does not change,
     *      meaning that no further values can be resolved using the current referred lines. Or the nonogram is solved
     *      (@see Nonogram::solved).
     * 3.   Adding a line to refer (@see Nonogram::add_line), if not all lines are already referred - go to step 2,
     *      otherwise - throw exception denoting the nonogram is not solvable.
     */
    void solve();

    /**
     * Save the current board state to a file.
     *
     * @param file_name The file to save the board state to.
     */
    void save(const string &file_name);

    /// Representation for the known black values.
    static const string color_black;

    /// Representation for the known white values.
    static const string color_white;

    /// Clear representation for the white values.
    static const string color_empty;

    /// Representation for the unknown values.
    static const string color_unknown;


private:

    /// The dimensions of the nonogram.
    int m_x, m_y;

    /// Whether the board values are known.
    vector<vector<bool>> m_set;

    /// The values of the board.
    vector<vector<bool>> m_value;

    /// The lines of the nonogram.
    vector<Line> m_lines;

    /// The board states.
    vector<Board> m_boards;

    /// The number of referred lines, lines that are not referred are used to resolve values for the line without
    /// actually generating all the line's possibilities, for referred lines the values are generated and reduced later.
    unsigned int m_referred_number;
};
