#include "Line.h"

Line::Line(const vector<int> &numbers, int size, Direction direction, int index) :
        m_numbers(numbers), m_size(size), m_direction(direction), m_index(index),
        m_max_combinations(get_combinations_number()) {}

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

inline size_t Line::max_combinations(const vector<vector<int>> &possible_per_number) const {
    vector<vector<size_t>> possibilities_count_per_number(m_numbers.size());
    for (int i = 0; i < possible_per_number.size(); i++) {
        possibilities_count_per_number[i].resize(possible_per_number[i].size());
    }

    for (int i = 0; i < possible_per_number[m_numbers.size() - 1].size(); i++) {
        possibilities_count_per_number[m_numbers.size() - 1][i] = possible_per_number[m_numbers.size() - 1].size() - i;
    }

    for (int i = m_numbers.size() - 2; i >= 0; i--) {
        for (int j = possible_per_number[i].size() - 1; j >= 0; j--) {
            int next_number_leftmost = lower_bound(possible_per_number[i + 1].begin(), possible_per_number[i + 1].end(),
                                                   possible_per_number[i][j] + m_numbers[i] + 1) -
                                       possible_per_number[i + 1].begin();
            size_t combinations = ((j == possible_per_number[i].size() - 1) ? 0 :
                                   possibilities_count_per_number[i][j + 1]) +
                                  possibilities_count_per_number[i + 1][next_number_leftmost];
            possibilities_count_per_number[i][j] = combinations;
        }
    }

    return possibilities_count_per_number[0][0];
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

#if DEBUG_LOGS
    Logger logger(INFO);
    auto &output = logger.Get();
    output << "Size: " << m_size << ", values:";
    for (const auto &value : m_numbers) output << " " << value;
    output << ", free space: " << free_space << ", known: ";
    for (const auto &value : known_colored) output << value;
    output << "." << endl;
#endif // DEBUG_LOGS

    return known_colored;
}

Known Line::initial_optimization() const {
    Known known;
    vector<bool> known_colored;

    known.first.resize(m_size, false);
    known.second.resize(m_size, false);

    if (m_numbers.empty()) {
#if DEBUG_LOGS
        Logger(DEBUG).Get() << "Size: " << m_size << ", all clear." << endl;
#endif // DEBUG_LOGS

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

vector<pair<int, int>> Line::get_limits(const vector<int> &possible_length_by_location) const {
    int number_index;

    vector<pair<int, int>> limits(m_numbers.size());

    // Fit left.
    number_index = 0;
    for (int i = 0; (i < m_size) && (number_index < m_numbers.size()); i++) {
        if (possible_length_by_location[i] >= m_numbers[number_index]) {
            limits[number_index].first = i;
            // Need space.
            i += m_numbers[number_index];
            number_index++;
        }
    }

    // Fit right.
    number_index = m_numbers.size() - 1;
    for (int i = m_size - 1; (i >= 0) && (number_index >= 0); i--) {
        if (possible_length_by_location[i] >= m_numbers[number_index]) {
            limits[number_index].second = i;
            number_index--;
            // Need space.
            i -= m_numbers[number_index];
        }
    }

#if DEBUG_TRACES
    Logger(TRACE).Get() << "Ranges:" << endl;
    for (int i = 0; i < limits.size(); i++) {
        Logger(TRACE).Get() << i << ": " << limits[i].first << "->" << limits[i].second << endl;
    }
#endif // DEBUG_TRACES

    return move(limits);
}

vector<set<int>> Line::possible_per_number_to_extended_place(const vector<vector<int>> &possible_per_number) const {
    vector<set<int>> extended_possible_per_place(m_size);
    for (int i = 0; i < m_numbers.size(); i++) {
        for (const auto &num : possible_per_number[i]) {
            for (int j = 0; j < m_numbers[i]; j++) extended_possible_per_place[num + j].insert(i);
        }
    }

    return move(extended_possible_per_place);
}

vector<vector<int>>
Line::possible_per_extended_place_to_number(const vector<set<int>> &extended_possible_per_place) const {
    vector<vector<int>> possible_per_number(m_numbers.size());
    vector<vector<int>> extended_possible_per_number(m_numbers.size());

    for (int i = 0; i < m_size; i++) {
        for (const auto &num : extended_possible_per_place[i]) {
            extended_possible_per_number[num].push_back(i);
        }
    }

    for (int i = 0; i < m_numbers.size(); i++) {
        int counter = 1;
        if (counter >= m_numbers[i]) {
            possible_per_number[i].push_back(extended_possible_per_number[i][0]);
        }
        for (int j = 1; j < extended_possible_per_number[i].size(); j++) {
            if (extended_possible_per_number[i][j] == extended_possible_per_number[i][j - 1] + 1) {
                counter++;
            } else {
                counter = 1;
            }
            if (counter >= m_numbers[i]) {
                possible_per_number[i].push_back(extended_possible_per_number[i][j] - m_numbers[i] + 1);
            }
        }
    }

    return move(possible_per_number);
}

void Line::reduce_by_required(vector<set<int>> &extended_possible_per_place, const vector<int> &required,
                              const vector<pair<int, int>> &limits) {
    for (const auto &single_required : required) {
        if (extended_possible_per_place[single_required].size() == 1) {
            int fitting = *extended_possible_per_place[single_required].begin();
            for (int j = limits[fitting].first; j < limits[fitting].second + m_numbers[fitting]; j++) {
                if ((j < (single_required - m_numbers[fitting] + 1)) || (j >= (single_required + m_numbers[fitting]))) {
                    auto t = find(extended_possible_per_place[j].begin(), extended_possible_per_place[j].end(),
                                  fitting);
                    if (t != extended_possible_per_place[j].end()) {
                        extended_possible_per_place[j].erase(t);
                    }
                }
            }
        }
    }
}

void Line::reduce_by_blocking_required(vector<vector<int>> &possible_per_number, const vector<int> &required) {
    for (const auto &single_required : required) {
        for (int i = 0; i < m_numbers.size(); i++) {
            possible_per_number[i].erase(
                    remove_if(possible_per_number[i].begin(), possible_per_number[i].end(), [&](int place) {
                        return ((place - 1 == single_required) ||
                                (place + m_numbers[i] == single_required)); // Includes trailing space.
                    }), possible_per_number[i].end());

        }
    }
}

void Line::reduce_by_single_fit(vector<vector<int>> &possible_per_number) {
    for (int i = 0; i < m_numbers.size(); i++) {
        if (possible_per_number[i].size() == 1) {
            int known_place = possible_per_number[i][0];
            for (int j = 0; j < m_numbers.size(); j++) {
                if (j == i) continue;
                possible_per_number[j].erase(
                        remove_if(possible_per_number[j].begin(), possible_per_number[j].end(), [&](int place) {
                            return ((place >= known_place) &&
                                    (place <= known_place + m_numbers[i])); // Includes trailing space.
                        }), possible_per_number[j].end());
            }
        }
    }
}

void Line::reduce_by_order(vector<vector<int>> &possible_per_number) {
    for (int i = m_numbers.size() - 2; i >= 0; i--) {
        int rightmost = possible_per_number[i + 1][possible_per_number[i + 1].size() - 1];
        possible_per_number[i].erase(
                remove_if(possible_per_number[i].begin(), possible_per_number[i].end(), [&](int place) {
                    return place + m_numbers[i] >= rightmost; // Includes trailing space.
                }), possible_per_number[i].end());
    }

    for (int i = 1; i < m_numbers.size(); i++) {
        int leftmost = possible_per_number[i - 1][0];
        possible_per_number[i].erase(
                remove_if(possible_per_number[i].begin(), possible_per_number[i].end(), [&](int place) {
                    return place <= leftmost + m_numbers[i - 1]; // Includes trailing space.
                }), possible_per_number[i].end());
    }
}

int Line::counts_elements(const vector<vector<int>> &value) {
    int elements = 0;

    for (const auto &values : value) elements += values.size();

    return elements;
}

vector<vector<int>> Line::generate_possible_per_number(const Known &known) {
    // Get the possible length for each initial location.
    vector<int> possible_length_by_location(m_size);
    int counter = 0;
    for (int i = m_size - 1; i >= 0; i--) {
        if (!known.first[i] || known.second[i]) {
            counter++;
        } else {
            counter = 0;
        }
        possible_length_by_location[i] = counter;
    }

    vector<pair<int, int>> limits = get_limits(possible_length_by_location);

    // Get known black.
    vector<int> required;
    for (int i = 0; i < m_size; i++) {
        if (known.first[i] && known.second[i]) {
            required.push_back(i);
        }
    }

    vector<vector<int>> possible_per_number(m_numbers.size());

    for (int i = 0; i < m_numbers.size(); i++) {
        for (int j = limits[i].first; j <= limits[i].second; j++) {
            if (m_numbers[i] <= possible_length_by_location[j]) {
                possible_per_number[i].push_back(j);
            }
        }
    }

    int elements = counts_elements(possible_per_number);
    vector<set<int>> extended_possible_per_place;
    int prev_elements;

    do {
        prev_elements = elements;

        extended_possible_per_place = possible_per_number_to_extended_place(possible_per_number);

        reduce_by_required(extended_possible_per_place, required, limits);

        possible_per_number = possible_per_extended_place_to_number(extended_possible_per_place);

        reduce_by_single_fit(possible_per_number);

        reduce_by_order(possible_per_number);

        reduce_by_blocking_required(possible_per_number, required);

        elements = counts_elements(possible_per_number);
    } while (elements < prev_elements);

    return move(possible_per_number);
}

Known Line::optimize_locations(Known known) {
    if (all_of(known.first.begin(), known.first.end(), [](bool v) { return v; })) {
        m_options = {known.second};
        m_max_combinations = 1;

        return move(known);
    }

    vector<vector<int>> possible_per_number = generate_possible_per_number(known);

#if DEBUG_LOGS
    Known old = known;
#endif // DEBUG_LOGS

    int leftmost, rightmost;
    for (int i = 0; i < m_numbers.size(); i++) {
        leftmost = possible_per_number[i][0];
        rightmost = possible_per_number[i][possible_per_number[i].size() - 1];
        for (int j = rightmost - leftmost; j < m_numbers[i]; j++) {
            known.first[j + leftmost] = true;
            known.second[j + leftmost] = true;
        }

        // Only one possible place, add padding.
        if (rightmost == leftmost) {
            if (leftmost > 0) {
                known.first[rightmost - 1] = true;
                known.second[rightmost - 1] = false;
            }
            if (leftmost + m_numbers[i] < m_size) {
                known.first[leftmost + m_numbers[i]] = true;
                known.second[leftmost + m_numbers[i]] = false;
            }
        }
    }

    // Leftmost padding.
    for (int i = 0; i < possible_per_number[0][0]; i++) {
        known.first[i] = true;
        known.second[i] = false;
    }
    // Rightmost padding.
    for (int i = possible_per_number[m_numbers.size() - 1][possible_per_number[m_numbers.size() - 1].size() - 1] +
                 m_numbers[m_numbers.size() - 1]; i < m_size; i++) {
        known.first[i] = true;
        known.second[i] = false;
    }
    // Padding between numbers.
    for (int i = 0; i < m_numbers.size() - 1; i++) {
        rightmost = possible_per_number[i][possible_per_number[i].size() - 1] + m_numbers[i];
        leftmost = possible_per_number[i + 1][0];
        for (int j = rightmost; j < leftmost; j++) {
            known.first[j] = true;
            known.second[j] = false;
        }
    }

#if DEBUG_LOGS
    if ((known.first != old.first) || (known.second != old.second)) {
        Logger logger(DEBUG);
        auto &output = logger.Get();
        output << "KNOWN OLD: ";
        for (int i = 0; i < old.first.size(); i++) {
            output << (old.first[i] ? (old.second[i] ? '1' : '0') : ' ');
        }
        output << endl;
        logger.flush();

        logger.init_line();
        output << "KNOWN NEW: ";
        for (int i = 0; i < known.first.size(); i++) {
            output << (known.first[i] ? (known.second[i] ? '1' : '0') : ' ');
        }
        output << endl;
        logger.flush();
    }
#endif // DEBUG_LOGS

    m_max_combinations = max_combinations(possible_per_number);

    return move(known);
}

vector<bool> Line::generate_option(const vector<int> &iterators, const vector<vector<int>> &possible_per_number) const {
    vector<bool> option(m_size, false);

    for (int i = 0; i < m_numbers.size(); i++) {
        for (int j = 0; j < m_numbers[i]; j++) option[possible_per_number[i][iterators[i]] + j] = true;
    }

#if DEBUG_TRACES
    Logger logger(TRACE);
    auto &output = logger.Get();
    for (auto &&tile : option) {
        output << (tile ? '1' : '0');
    }
    output << endl;
    logger.flush();
#endif //DEBUG_TRACES

    return move(option);
}

inline bool Line::check_option_by_required(const vector<int> &required, const vector<bool> &option) {
    return all_of(required.begin(), required.end(), [&option](int single_required) { return option[single_required]; });
}

void Line::generate_options(const Known &known) {
#if DEBUG_LOGS
    Logger(DEBUG).Get() << "Originally possible " << get_combinations_number() << " options" << endl;
#endif // DEBUG_LOGS

    if (all_of(known.first.begin(), known.first.end(), [](bool v) { return v; })) {
        m_options = {known.second};

#if DEBUG_LOGS
        Logger(DEBUG).Get() << "All known" << endl;
#endif // DEBUG_LOGS

        return;
    }

    vector<vector<int>> possible_per_number = generate_possible_per_number(known);

    // Get known black.
    vector<int> required;
    for (int i = 0; i < m_size; i++) {
        if (known.first[i] && known.second[i]) {
            required.push_back(i);
        }
    }

    vector<int> iterators(m_numbers.size(), 0);
    int index;
    int generated = 0;
    while (true) {
        auto option = generate_option(iterators, possible_per_number);
        generated++;

        if (check_option_by_required(required, option)) {
            m_options.push_back(move(option));
        }

        for (index = m_numbers.size() - 1;
             (index >= 0) && (iterators[index] == possible_per_number[index].size() - 1); index--);
        if (index < 0) break;

        iterators[index]++;
        index++;
        for (; index < m_numbers.size(); index++) {
            int lower_limit = possible_per_number[index - 1][iterators[index - 1]] + m_numbers[index - 1] + 1;
            auto t = lower_bound(possible_per_number[index].begin(), possible_per_number[index].end(), lower_limit);
            iterators[index] = t - possible_per_number[index].begin();
        }
    }

#if DEBUG_LOGS
    Logger(DEBUG).Get() << "Generated " << m_options.size() << " options (saved "
                        << get_combinations_number() - generated << " by optimization and "
                        << generated - m_options.size() << " by filtering)" << endl;
#endif // DEBUG_LOGS
}

inline bool Line::check_option(const Known &known, const vector<bool> &option) {
    for (int i = 0; i < known.first.size(); i++) {
        if (known.first[i]) {
            if (option[i] != known.second[i]) {
#if DEBUG_TRACES
                Logger logger(TRACE);
                auto &output = logger.Get();
                Logger(TRACE).Get() << "DIFF:   " << string(i, ' ') << known.second[i] << " [" << i << "]" << endl;
                output << "DELETE: ";
                for (int j = 0; j < known.first.size(); j++) {
                    output << option[j];
                }
                output << endl;
#endif // DEBUG_TRACES

                return false;
            }
        }
    }
    return true;
}

void Line::filter(const Known &known) {
    int previous_size = m_options.size();

    if (previous_size == 1) {
        return;
    }

#if DEBUG_TRACES
    Logger logger(TRACE);
    auto &output = logger.Get();
    output << "KNOWN:  ";
    for (int i = 0; i < known.first.size(); i++) {
        output << (known.first[i] ? (known.second[i] ? '1' : '0') : ' ');
    }
    output << endl;
    logger.flush();
#endif //DEBUG_TRACES

    m_options.erase(remove_if(m_options.begin(), m_options.end(), [&known](const vector<bool> &option) {
        return !check_option(known, option);
    }), m_options.end());

#if DEBUG_LOGS
    Logger(DEBUG).Get() << "Filter options " << previous_size << " -> " << m_options.size() << endl;
#endif // DEBUG_LOGS
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

#if DEBUG_LOGS
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
#endif // DEBUG_LOGS

    for (i = 0; i < known.first.size(); i++) {
        if (merge_and[i] == merge_or[i]) {
            known.first[i] = true;
            known.second[i] = merge_and[i];
        }
    }

    return move(known);
}
