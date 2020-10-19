#include "Nonogram.h"

const string Nonogram::color_black = "@@@";
const string Nonogram::color_white = "...";
const string Nonogram::color_empty = "   ";
const string Nonogram::color_unknown = "???";

Nonogram::Nonogram(const string &file_name) : m_referred_number(0) {
    ifstream board(file_name);
    string line;

    if (board.fail()) {
        // Assume it's because the file does not exist.
        throw runtime_error("Input file does not exist.");
    }

    getline(board, line);
    istringstream is_dimensions(line);

    // Get the nonogram's dimensions.
    is_dimensions >> m_x >> m_y;

    int i, n = 0, sum = 0;

    vector<vector<int>> vertical(m_x);
    vector<vector<int>> horizontal(m_y);

#if DEBUG_SUMMARY
    Logger(ERROR).get() << "Board size " << m_x << " X " << m_y << endl;
#endif // DEBUG_SUMMARY

    // Get vertical lines values.
    for (i = 0; i < m_x; i++) {
        if (!getline(board, line)) {
            board.close();
            throw runtime_error("Not enough lines in input.");
        }

        // Ignore lines that start with "*".
        if (line[0] == '*') {
            i--;
            continue;
        }

        istringstream is_line(line);
        while (is_line >> n) {
            vertical[i].push_back(n);
            sum += n;
        }
        m_lines.emplace_back(Line(vertical[i], m_y, VERTICAL, i));
    }

    // Get horizontal lines values.
    for (i = 0; i < m_y; i++) {
        if (!getline(board, line)) {
            board.close();
            throw runtime_error("Not enough lines in input.");
        }

        // Ignore lines that start with "*".
        if (line[0] == '*') {
            i--;
            continue;
        }

        istringstream is_line(line);
        while (is_line >> n) {
            horizontal[i].push_back(n);
            sum -= n;
        }
        m_lines.emplace_back(Line(horizontal[i], m_x, HORIZONTAL, i));
    }

    board.close();

    // Validate the vertical and horizontal numbers sum.
    if (sum != 0) {
        throw runtime_error("Invalid input, numbers sum is incorrect.");
    }

    // Set up the board with all values as unknown.
    m_value.resize(m_x);
    m_set.resize(m_x);
    for (i = 0; i < m_x; m_value[i].resize(m_y, false), m_set[i].resize(m_y, false), i++);

    save_board();
}

void Nonogram::save_board() {
    m_boards.emplace_back(m_set, m_value);
}

const Board &Nonogram::get_board(int index) const {
    if (index < 0) {
        index += m_boards.size();
    }
    return m_boards[index];
}

unsigned int Nonogram::count_known(int index) const {
    const Board &board = get_board(index);

    unsigned int count = 0;
    for (int i = 0; i < m_x; i++) {
        for (int j = 0; j < m_y; j++) {
            if (board.first[i][j]) count++;
        }
    }

    return count;
}

void Nonogram::print_board(LogLevel level, const string &black, const string &white, const string &unknown) const {
    string representation;
    string line;

    for (int i = 0; i < m_y; i++) {
        line = "";
        for (int j = 0; j < m_x; j++) {
            representation = m_set[j][i] ? (m_value[j][i] ? black : white) : unknown;
            line += representation;
        }
        Logger(level).get() << line << endl;
        Logger(level).flush();
    }
}

Known Nonogram::get_board_horizontal(unsigned int y, const Board &board) const {
    vector<bool> set(m_x), value(m_x);

    for (int i = 0; i < m_x; i++) {
        set[i] = board.first[i][y];
        value[i] = board.second[i][y];
    }

    return {set, value};
}

Known Nonogram::get_board_vertical(unsigned int x, const Board &board) const {
    vector<bool> set(m_y), value(m_y);

    for (int i = 0; i < m_y; i++) {
        set[i] = board.first[x][i];
        value[i] = board.second[x][i];
    }

    return {set, value};
}

Known Nonogram::get_horizontal(unsigned int y) const {
    return move(get_board_horizontal(y, {m_set, m_value}));
}

Known Nonogram::get_vertical(unsigned int x) const {
    return move(get_board_vertical(x, {m_set, m_value}));
}

void Nonogram::set_horizontal(const Known &known, unsigned int y) {
    for (int i = 0; i < m_x; i++) {
        if (known.first[i]) {
            m_set[i][y] = known.first[i];
            m_value[i][y] = known.second[i];
        }
    }
}

void Nonogram::set_vertical(const Known &known, unsigned int x) {
    for (int i = 0; i < m_y; i++) {
        if (known.first[i]) {
            m_set[x][i] = known.first[i];
            m_value[x][i] = known.second[i];
        }
    }
}

bool Nonogram::horizontal_changed(unsigned int y, int first, int second) const {
    Known first_board = get_board_horizontal(y, get_board(first));
    Known second_board = get_board_horizontal(y, get_board(second));
    for (int i = 0; i < m_x; i++) {
        if (first_board.first[i] != second_board.first[i]) {
            return true;
        }
    }
    return false;
}

bool Nonogram::vertical_changed(unsigned int x, int first, int second) const {
    Known first_board = get_board_vertical(x, get_board(first));
    Known second_board = get_board_vertical(x, get_board(second));
    for (int i = 0; i < m_y; i++) {
        if (first_board.first[i] != second_board.first[i]) {
            return true;
        }
    }
    return false;
}

void Nonogram::initial_optimization() {
    for (const auto &line : m_lines) {
        if (line.m_direction == HORIZONTAL) {
            set_horizontal(line.initial_optimization(), line.m_index);
        } else {
            set_vertical(line.initial_optimization(), line.m_index);
        }
    }
}

void Nonogram::add_line() {
    // Sort lines by maximum number of combinations.
    sort(m_lines.begin() + m_referred_number, m_lines.end(), [](const Line &lhs, const Line &rhs) {
        return lhs.m_max_combinations < rhs.m_max_combinations;
    });

    // Take the line with the least number of combinations.
    auto &line = m_lines[m_referred_number];

#if DEBUG_LOGS
    Logger(INFO).get() << "Adding line (" << ((line.m_direction == HORIZONTAL) ? "horizontal" : "vertical") << " "
                       << line.m_index << ") " << m_referred_number << " -> " << m_referred_number + 1 << endl;
#endif // DEBUG_LOGS

    // Generate the options for the line.
    line.generate_options((line.m_direction == HORIZONTAL) ? get_horizontal(line.m_index) : get_vertical(line.m_index));

    // Extract known values from the line.
    if (line.m_direction == HORIZONTAL) {
        auto known = line.merge_options(get_horizontal(line.m_index));
        set_horizontal(known, line.m_index);
    } else {
        auto known = line.merge_options(get_vertical(line.m_index));
        set_vertical(known, line.m_index);
    }

    m_referred_number++;
}

bool Nonogram::solved() {
    for (int i = 0; i < m_x; i++) {
        for (int j = 0; j < m_y; j++) {
            if (!m_set[i][j]) return false;
        }
    }
    return true;
}

void Nonogram::step() {
    Known known;

    // Extract known values from lines that are not yet resolved, and set them in the board.
    for (unsigned int i = m_referred_number; i < m_x + m_y; i++) {
        auto &line = m_lines[i];
        if (line.m_direction == HORIZONTAL) {
            if (horizontal_changed(line.m_index)) {
                set_horizontal(line.calculate_known(get_horizontal(line.m_index)), line.m_index);
            }
        } else if (vertical_changed(line.m_index)) {
            set_vertical(line.calculate_known(get_vertical(line.m_index)), line.m_index);
        }
    }

    // Filter the resolved lines options by what is already known of the line.
    for (unsigned int i = 0; i < m_referred_number; i++) {
        auto &line = m_lines[i];
        if (line.m_direction == HORIZONTAL) {
            if (horizontal_changed(line.m_index)) {
                line.filter(get_horizontal(line.m_index));
            }
        } else if (vertical_changed(line.m_index)) {
            line.filter(get_vertical(line.m_index));
        }
    }

    // Extract known values from resolved lines, and set them in the board.
    for (unsigned int i = 0; i < m_referred_number; i++) {
        auto &line = m_lines[i];
        if (line.m_direction == HORIZONTAL) {
            if (horizontal_changed(line.m_index)) {
                known = line.merge_options(get_horizontal(line.m_index));
                set_horizontal(known, line.m_index);
            }
        } else if (vertical_changed(line.m_index)) {
            known = line.merge_options(get_vertical(line.m_index));
            set_vertical(known, line.m_index);
        }
    }
}

void Nonogram::solve() {
    initial_optimization();

#if DEBUG_LOGS
    Logger(INFO).get() << endl;
    Logger(INFO).get() << "Initial Optimization" << endl;
    print_board();
    Logger(INFO).get() << "Known count: " << count_known(-1) << endl;
#endif // DEBUG_LOGS

    save_board();

    for (int i = 0; !solved(); i++) {
        step();

#if DEBUG_LOGS
        Logger(INFO).get() << endl;
        Logger(INFO).get() << "Step " << i + 1 << endl;
        print_board();
        Logger(INFO).get() << "Known count: " << count_known(-1) << endl;

        size_t options_count = 0;
        for (const auto &line : m_lines) {
            options_count += line.m_options.size();
        }
        Logger(INFO).get() << "Options count: " << options_count << endl;
#endif // DEBUG_LOGS
        save_board();

        if (count_known(-1) <= count_known(-2)) {
            if (m_referred_number == m_lines.size()) {
                // If all lines are referred, and no more values of the board can resolved, raise an exception.
                throw runtime_error("The nonogram may have more than one solution.");
            } else {
                // If not all lines are referred, and no more values of the board can resolved, add a new line to refer.
                add_line();
            }
        }
    }
}

void Nonogram::save(const string &file_name) {
    ofstream board(file_name);

    for (int i = 0; i < m_y; i++) {
        for (int j = 0; j < m_x; j++) {
            board << (m_set[j][i] ? (m_value[j][i] ? color_black : color_empty) : color_unknown);
        }
        board << endl;
    }

    board.close();
}
