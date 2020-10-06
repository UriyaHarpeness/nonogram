#include "Line.h"

Line::Line(const vector<int> &numbers, int size, Direction direction, int index) :
        m_numbers(numbers), m_size(size), m_direction(direction), m_index(index) {}

size_t Line::n_choose_k(unsigned int n, unsigned int k) {
    if (k > n) return 0;
    if (k * 2 > n) k = n - k;
    if (k == 0) return 1;

    size_t result = n;
    for (unsigned int i = 2; i <= k; ++i) {
        result *= (n - i + 1);
        result /= i;
    }
    return result;
}

size_t Line::calculate_combinations(unsigned int n, unsigned int k) {
    return n_choose_k(n + k, k);
}

size_t Line::get_combinations_number() const {
    return calculate_combinations(get_free_space(), m_numbers.size());
}

int Line::get_free_space() const {
    int values_sum = 0;
    for (const auto &value : m_numbers) values_sum += value;
    return m_size - (values_sum + (int) m_numbers.size() - 1);
}

vector<bool> Line::get_known_colored() const {
    vector<bool> known_colored(m_size, false);

    int offset = 0;
    int free_space = get_free_space();
    for (const auto &value : m_numbers) {
        for (int i = free_space; i < value; i++) known_colored[offset + i] = true;
        offset += value + 1;
    }

    Logger logger(INFO);
    auto &output = logger.Get();
    output << "Size: " << m_size << ", values:";
    for (const auto &value : m_numbers) output << " " << value;
    output << ", free space: " << free_space << ", known: ";
    for (const auto &value : known_colored) output << value;
    output << "." << endl;

    return known_colored;
}

Known Line::initial_optimization() const {
    Known known;
    vector<bool> known_colored;

    known.first.resize(m_size, false);
    known.second.resize(m_size, false);

    if (m_numbers.empty()) {
        Logger(DEBUG).Get() << "Size: " << m_size << ", all clear." << endl;
        for (int i = 0; i < m_size; i++) {
            known.first[i] = true;
            known.second[i] = false;
        }

        return known;
    }

    known_colored = get_known_colored();
    int fixed_place = get_free_space() == 0;
    for (int i = 0; i < m_size; i++) {
        if (known_colored[i] || fixed_place) {
            known.first[i] = true;
            known.second[i] = known_colored[i];
        }
    }

    return known;
}

vector<bool> Line::generate_option(const vector<int> &spaces) {
    vector<bool> option(m_size, true);

    // Spaces is a sorted list of the spaces locations.
    int index = 0, space_index = 0;
    for (int i = 0; i < m_numbers.size(); i++) {
        while ((space_index < spaces.size()) && (spaces[space_index] == i)) {
            option[index++] = false;
            space_index++;
        }
        index += m_numbers[i];

        if (i < m_numbers.size()) {
            option[index] = false;
        }
        index++;
    }
    for (; index < m_size; index++) option[index] = false;

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

void Line::generate_options(const Known &known) {
    int free_space = get_free_space(), move_index;

    vector<int> options_iteration(free_space, 0);

    Logger(DEBUG).Get() << "Expected " << get_combinations_number() << " options" << endl;

    if (free_space == 0) {
        assert(all_of(known.first.begin(), known.first.end(), [](bool v) { return v; }));

        m_options = {known.second};

        return;
    }

    while (true) {
        m_options.push_back(move(generate_option(options_iteration)));

        for (move_index = free_space - 1;
             (move_index >= 0) && (options_iteration[move_index] > m_numbers.size() - 1); move_index--);
        if ((move_index < 0) || m_numbers.empty()) break;
        int new_value = options_iteration[move_index] + 1;
        for (; move_index < free_space; move_index++) options_iteration[move_index] = new_value;
    }

    Logger(DEBUG).Get() << "Generated " << m_options.size() << " options" << endl;
}

void Line::filter(const Known &known) {
    int previous_size = m_options.size();

//    Logger logger(TRACE);
//    auto &output = logger.Get();
//    output << "KNOWN:  ";
//    for (int i = 0; i < known.first.size(); i++) {
//        output << (known.first[i] ? (known.second[i] ? '1' : '0') : ' ');
//    }
//    output << endl;
//    logger.flush();

    m_options.erase(remove_if(m_options.begin(), m_options.end(), [&known](const vector<bool> &option) {
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
    }), m_options.end());
    Logger(DEBUG).Get() << "Filter options " << previous_size << " -> " << m_options.size() << endl;
}

Known Line::merge_options(Known known) {
    vector<bool> merge_and(known.first.size(), false), merge_or(known.first.size(), false);
    bool and_flag, or_flag;
    int i, j;

    for (i = 0; i < m_size; i++) {
        if (known.first[i]) {
            and_flag = known.second[i];
            or_flag = known.second[i];
        } else {
            and_flag = m_options[0][i];
            or_flag = m_options[0][i];

            for (j = 1; j < m_options.size(); j++) {
                and_flag = and_flag & m_options[j][i];
                or_flag = or_flag | m_options[j][i];

                if (and_flag ^ or_flag) {
                    break;
                }
            }
        }
        merge_and[i] = and_flag;
        merge_or[i] = or_flag;
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
