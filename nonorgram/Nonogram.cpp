#include "Nonogram.h"

const string Nonogram::color_black = "@@@";
const string Nonogram::color_white = "...";
const string Nonogram::color_empty = "   ";
const string Nonogram::color_unknown = "???";

Nonogram::Nonogram(const string &file_name) {
    ifstream board(file_name);
    string line;

    if (board.fail()) {
        // Assume it's because the file does not exist.
        throw runtime_error("Input file does not exist.");
    }

    getline(board, line);
    istringstream is_dimensions(line);

    is_dimensions >> m_x >> m_y;

    int i, n, sum = 0;

    m_vertical.resize(m_x);
    m_horizontal.resize(m_y);

    Logger(ERROR).Get() << "Board size " << m_x << " X " << m_y << endl;

    for (i = 0; i < m_x; i++) {
        if (!getline(board, line)) {
            board.close();
            throw runtime_error("Not enough lines in input.");
        }

        if (line[0] == '*') {
            i--;
            continue;
        }

        istringstream is_line(line);
        while (is_line >> n) {
            m_vertical[i].push_back(n);
            sum += n;
        }
    }
    for (i = 0; i < m_y; i++) {
        if (!getline(board, line)) {
            board.close();
            throw runtime_error("Not enough lines in input.");
        }

        if (line[0] == '*') {
            i--;
            continue;
        }

        istringstream is_line(line);
        while (is_line >> n) {
            m_horizontal[i].push_back(n);
            sum -= n;
        }
    }

    board.close();

    if (sum != 0) {
        throw runtime_error("Invalid input, numbers sum is incorrect.");
    }

    m_value.resize(m_x);
    m_set.resize(m_x);
    for (i = 0; i < m_x; m_value[i].resize(m_y, false), m_set[i].resize(m_y, false), i++);

    save_board();
}

void Nonogram::save_board() {
    m_boards.emplace_back(m_set, m_value);
}

const Board &Nonogram::get_board(int index) {
    if (index < 0) {
        index += m_boards.size();
    }
    return m_boards[index];
}

unsigned int Nonogram::count_known(int index) {
    const Board &board = get_board(index);

    unsigned int count = 0;
    for (int i = 0; i < m_x; i++) {
        for (int j = 0; j < m_y; j++) {
            if (board.first[i][j]) count++;
        }
    }

    return count;
}

unsigned int Nonogram::n_choose_k(unsigned int n, unsigned int k) {
    if (k > n) return 0;
    if (k * 2 > n) k = n - k;
    if (k == 0) return 1;

    unsigned int result = n;
    for (unsigned int i = 2; i <= k; ++i) {
        result *= (n - i + 1);
        result /= i;
    }
    return result;
}

unsigned int Nonogram::calculate_combinations(unsigned int n, unsigned int k) {
    return n_choose_k(n + k - 1, k - 1);
}

int Nonogram::get_free_space(const vector<int> &values, int size) {
    int values_sum = 0;
    for (const auto &value : values) values_sum += value;
    return size - (values_sum + (int) values.size() - 1);
}

vector<bool> Nonogram::get_known_colored(const vector<int> &values, int size) {
    vector<bool> known_colored(size, false);

    int offset = 0;
    int free_space = get_free_space(values, size);
    for (const auto &value : values) {
        for (int i = free_space; i < value; i++) known_colored[offset + i] = true;
        offset += value + 1;
    }

    Logger logger(INFO);
    auto &output = logger.Get();
    output << "Size: " << size << ", values:";
    for (const auto &value : values) output << " " << value;
    output << ", free space: " << free_space << ", known: ";
    for (const auto &value : known_colored) output << value;
    output << "." << endl;

    return known_colored;
}

void Nonogram::initial_optimization() {
    vector<bool> known_colored;

    int i, j;
    bool fixed_place;

    for (i = 0; i < m_y; i++) {
        Logger(DEBUG).Get() << "Optimizing horizontal " << i << endl;
        if (m_horizontal[i].empty()) {
            Logger(DEBUG).Get() << "Size: " << m_x << ", all clear." << endl;
            for (j = 0; j < m_x; j++) {
                m_set[j][i] = true;
                m_value[j][i] = false;
            }
            continue;
        }

        known_colored = get_known_colored(m_horizontal[i], m_x);
        fixed_place = get_free_space(m_horizontal[i], m_x) == 0;
        for (j = 0; j < m_x; j++) {
            if (known_colored[j] || fixed_place) {
                m_set[j][i] = true;
                m_value[j][i] = known_colored[j];
            }
        }
    }

    for (i = 0; i < m_x; i++) {
        Logger(DEBUG).Get() << "Optimizing vertical " << i << endl;
        if (m_vertical[i].empty()) {
            Logger(DEBUG).Get() << "Size: " << m_y << ", all clear." << endl;
            for (j = 0; j < m_y; j++) {
                m_set[i][j] = true;
                m_value[i][j] = false;
            }
            continue;
        }

        known_colored = get_known_colored(m_vertical[i], m_y);
        fixed_place = get_free_space(m_vertical[i], m_y) == 0;
        for (j = 0; j < m_y; j++) {
            if (known_colored[j] || fixed_place) {
                m_set[i][j] = true;
                m_value[i][j] = known_colored[j];
            }
        }
    }
}

void Nonogram::print_board(LogLevel level, const string &black, const string &white, const string &unknown) {
    string representation;
    string line;

    for (int i = 0; i < m_y; i++) {
        line = "";
        for (int j = 0; j < m_x; j++) {
            representation = m_set[j][i] ? (m_value[j][i] ? black : white) : unknown;
            line += representation;
        }
        Logger(level).Get() << line << endl;
        Logger(level).flush();
    }
}

Known Nonogram::get_horizontal(int y) {
    return move(get_board_horizontal(y, {m_set, m_value}));
}

Known Nonogram::get_vertical(int x) {
    return move(get_board_vertical(x, {m_set, m_value}));
}

Known Nonogram::get_board_horizontal(int y, const Board &board) {
    vector<bool> set(m_x), value(m_x);

    for (int i = 0; i < m_x; i++) {
        set[i] = board.first[i][y];
        value[i] = board.second[i][y];
    }

    return {set, value};
}

Known Nonogram::get_board_vertical(int x, const Board &board) {
    vector<bool> set(m_y), value(m_y);

    for (int i = 0; i < m_y; i++) {
        set[i] = board.first[x][i];
        value[i] = board.second[x][i];
    }

    return {set, value};
}


void Nonogram::set_horizontal(const Known &known, int y) {
    for (int i = 0; i < m_x; i++) {
        m_set[i][y] = known.first[i];
        m_value[i][y] = known.second[i];
    }
}

void Nonogram::set_vertical(const Known &known, int x) {
    for (int i = 0; i < m_y; i++) {
        m_set[x][i] = known.first[i];
        m_value[x][i] = known.second[i];
    }
}

bool Nonogram::horizontal_changed(int y, int first, int second) {
    Known first_board = get_board_horizontal(y, get_board(first));
    Known second_board = get_board_horizontal(y, get_board(second));
    for (int i = 0; i < m_x; i++) {
        if (first_board.first[i] != second_board.first[i]) {
            return true;
        }
    }
    return false;
}

bool Nonogram::vertical_changed(int x, int first, int second) {
    Known first_board = get_board_vertical(x, get_board(first));
    Known second_board = get_board_vertical(x, get_board(second));
    for (int i = 0; i < m_y; i++) {
        if (first_board.first[i] != second_board.first[i]) {
            return true;
        }
    }
    return false;
}

vector<bool> Nonogram::generate_option(const vector<int> &numbers, const vector<int> &spaces, int size) {
    vector<bool> option(size, true);

    // Spaces is a sorted list of the spaces locations.
    int index = 0, space_index = 0;
    for (int i = 0; i < numbers.size(); i++) {
        while ((space_index < spaces.size()) && (spaces[space_index] == i)) {
            option[index++] = false;
            space_index++;
        }
        index += numbers[i];

        if (i < numbers.size()) {
            option[index] = false;
        }
        index++;
    }
    for (; index < size; index++) option[index] = false;

//    Logger logger(TRACE);
//    auto &output = logger.Get();
//
//    output << "Spaces indexes: ";
//    for (int i = 0; i < spaces.size(); i++) output << spaces[i] << " ";
//    output << endl;
//    logger.flush();
//    logger.init_line();
//
//    output << "Line: ";
//    for (int i = 0; i < option.size(); i++) output << option[i];
//    output << endl;

    return move(option);
}

vector<vector<bool>> Nonogram::generate_options(const Known &known, const vector<int> &numbers) {
    int free_space = get_free_space(numbers, known.first.size()), move_index;

    vector<int> options_iteration(free_space, 0);

    if (free_space == 0) {
        assert(all_of(known.first.begin(), known.first.end(), [](bool v) { return v; }));

        return {known.second};
    }

    Logger(DEBUG).Get() << "Expected " << calculate_combinations(free_space, numbers.size() + 1) << " options" << endl;

    vector<vector<bool>> options;

    while (true) {
        options.push_back(move(generate_option(numbers, options_iteration, known.first.size())));

        for (move_index = free_space - 1;
             (move_index >= 0) && (options_iteration[move_index] > numbers.size() - 1); move_index--);
        if ((move_index < 0) || numbers.empty()) break;
        int new_value = options_iteration[move_index] + 1;
        for (; move_index < free_space; move_index++) options_iteration[move_index] = new_value;
    }

    Logger(DEBUG).Get() << "Generated " << options.size() << " options" << endl;

    return move(options);
}

void Nonogram::initialize_options() {
    for (int i = 0; i < m_y; i++) {
        Logger(DEBUG).Get() << "Initialize options horizontal " << i << endl;
        m_horizontal_options.push_back(move(generate_options(get_horizontal(i), m_horizontal[i])));
    }
    for (int i = 0; i < m_x; i++) {
        Logger(DEBUG).Get() << "Initialize options vertical " << i << endl;
        m_vertical_options.push_back(move(generate_options(get_vertical(i), m_vertical[i])));
    }
}

bool Nonogram::solved() {
    for (int i = 0; i < m_x; i++) {
        for (int j = 0; j < m_y; j++) {
            if (!m_set[i][j]) return false;
        }
    }
    return true;
}

void Nonogram::filter(const Known &known, vector<vector<bool>> &options) {
    int previous_size = options.size();

//    Logger logger(TRACE);
//    auto &output = logger.Get();
//    output << "KNOWN:  ";
//    for (int i = 0; i < known.first.size(); i++) {
//        output << (known.first[i] ? (known.second[i] ? '1' : '0') : ' ');
//    }
//    output << endl;
//    logger.flush();

    options.erase(remove_if(options.begin(), options.end(), [&known](const vector<bool> &option) {
        for (int i = 0; i < known.first.size(); i++) {
            if (known.first[i]) {
                if (option[i] != known.second[i]) {
//                    // todo: enable to remove all trace logs in compile time maybe, takes a lot of time.
//                    Logger logger(TRACE);
//                    auto &output = logger.Get();
//                    Logger(TRACE).Get() << "DIFF:   " << string(i, ' ') << known.second[i] << " [" << i << "]" << endl;
//                    output << "DELETE: ";
//                    for (int j = 0; j < known.first.size(); j++) {
//                        output << option[j];
//                    }
//                    output << endl;
                    return true;
                }
            }
        }
        return false;
    }), options.end());
    Logger(DEBUG).Get() << "Filter options " << previous_size << " -> " << options.size() << endl;
}

Known Nonogram::merge_options(Known known, vector<vector<bool>> &options) {
    vector<bool> merge_and(known.first.size(), false), merge_or(known.first.size(), false);
    bool flag;
    int i, j;

    for (i = 0; i < options[0].size(); i++) {
        if (known.first[i]) {
            flag = known.second[i];
        } else {
            flag = options[0][i];
            for (j = 1; j < options.size(); j++) {
                flag = flag & options[j][i];
            }
        }
        merge_and[i] = flag;
    }

    for (i = 0; i < options[0].size(); i++) {
        if (known.first[i]) {
            flag = known.second[i];
        } else {
            flag = options[0][i];
            for (j = 1; j < options.size(); j++) {
                flag = flag | options[j][i];
            }
        }
        merge_or[i] = flag;
    }


    string line;

    line = "AND ";
    for (const auto &it : merge_and) line += (it ? '1' : '0');
    Logger(DEBUG).Get() << line << endl;

    line = "OR  ";
    for (const auto &it : merge_or) line += (it ? '1' : '0');
    Logger(DEBUG).Get() << line << endl;

    line = "SUM ";
    for (i = 0; i < known.first.size(); i++) {
        line += ((merge_and[i] == merge_or[i]) ? (merge_and[i] ? '1' : '0') : ' ');
    }
    Logger(DEBUG).Get() << line << endl;

    for (i = 0; i < known.first.size(); i++) {
        if (merge_and[i] == merge_or[i]) {
            known.first[i] = true;
            known.second[i] = merge_and[i];
        }
    }

    return move(known);
}

void Nonogram::step() {
    Known known;

    for (int i = 0; i < m_y; i++) {
        Logger(DEBUG).Get() << "Filter horizontal " << i << endl;
        if (!horizontal_changed(i)) {
            Logger(DEBUG).Get() << "Skipped" << endl;
            continue;
        }
        filter(get_horizontal(i), m_horizontal_options[i]);
    }
    for (int i = 0; i < m_x; i++) {
        Logger(DEBUG).Get() << "Filter vertical " << i << endl;
        if (!vertical_changed(i)) {
            Logger(DEBUG).Get() << "Skipped" << endl;
            continue;
        }
        filter(get_vertical(i), m_vertical_options[i]);
    }

    for (int i = 0; i < m_y; i++) {
        Logger(DEBUG).Get() << "Merge horizontal " << i << endl;
        if (!horizontal_changed(i)) {
            Logger(DEBUG).Get() << "Skipped" << endl;
            continue;
        }
        known = merge_options(get_horizontal(i), m_horizontal_options[i]);
        set_horizontal(known, i);
    }
    for (int i = 0; i < m_x; i++) {
        Logger(DEBUG).Get() << "Merge vertical " << i << endl;
        if (!vertical_changed(i)) {
            Logger(DEBUG).Get() << "Skipped" << endl;
            continue;
        }
        known = merge_options(get_vertical(i), m_vertical_options[i]);
        set_vertical(known, i);
    }
}

void Nonogram::solve() {
    initial_optimization();

    Logger(INFO).Get() << endl;
    Logger(INFO).Get() << "Initial Optimization" << endl;
    save_board();
    print_board();
    Logger(INFO).Get() << "Known count: " << count_known(-1) << endl;

    initialize_options();

    // todo: add check for unsolvable. (no change in known).
    for (int i = 0; !solved(); i++) {
        step();
        Logger(INFO).Get() << endl;
        Logger(INFO).Get() << "Step " << i + 1 << endl;
        save_board();
        print_board();
        Logger(INFO).Get() << "Known count: " << count_known(-1) << endl;

        if (count_known(-1) <= count_known(-2)) {
            throw runtime_error("The nonogram is unsolvable (at least not without guessing).");
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
