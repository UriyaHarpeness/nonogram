#include "Nono.h"

vector <int> Nono::split(string s, string token)
{
	vector <int> elements;
	int pos = 0;
	while (pos != -1)
	{
		pos = s.find(token);
		if (pos != -1)
		{
			elements.push_back(atoi(s.substr(0, pos).c_str()));
			s = s.substr(pos + 1, s.length() - pos - 1);
		}
		else
		{
			elements.push_back(stoi(s.c_str()));
		}
	}

	return elements;
}

Nono::Nono()
{
	time_t t = time(0);
	struct tm * tmp_t = localtime(&t);
	
	Nono::_board;
	Nono::_params;
	Nono::_size;
	Nono::_options;
	Nono::_show_stuff;
	Nono::_time_started = tmp_t->tm_sec + tmp_t->tm_min * 60 + tmp_t->tm_hour * 3600;
	Nono::_time_previous = _time_started;

	ifstream f("nonogram.txt");
	string s;
	int i;
	vector <int> tmp;
	
	getline(f, s);
	tmp = split(s, ",");
	_size.push_back(tmp[0]);
	_size.push_back(tmp[1]);

	tmp.clear();
	for (i = 0; i < _size[0]; i++)
	{
		tmp.push_back(1);
	}

	for(i = 0; i < _size[1]; i++)
	{
		_board.push_back(tmp);
	}

	i = 0;
	while (i < _size[0] + _size[1])
	{
		getline(f, s);
		i++;
		cout << i << " - " << s << endl;
		_params.push_back(split(s, ","));
	}

	f.close();
}

Nono::~Nono()
{

}

bool Nono::solve(bool show, bool show_2)
{
	int i, j;
	int size;
	int move;
	vector <int> line;
	vector <vector <int>> prev_board = _board;
	bool solveable = true;
	vector <int> ops_sizes;

	vector <int> comp1;
	vector <int> comp2;

	vector<int> smalls;
	
	_show_stuff = show;

	for (i = 0; i < _params.size(); i++)
	{
		size = (i < _size[0]) ? _size[1] : _size[0];
		move = movable(_params[i], size);
		if (show_2)
		{
			if (i < _size[0])
			{
				cout << "vars of coll: " << i + 1 << ", ";
			}
			else
			{
				cout << "vars of line: " << i + 1 - _size[0] << ", ";
			}
			for (j = 0; j < _params[i].size(); j++)
			{
				cout << _params[i][j] << ", ";
			}
			cout << "combos = " << combinum(move, _params[i].size() + 1) << endl;
		}
		_options.push_back(make_lines_vec(vars(move, _params[i].size() + 1), _params[i], size));
		ops_sizes.push_back(0);
	}

	if (show_2)
		cout << previous_time() << endl;
	cout << "--- stages ---" << endl;



	j = 0;
	show_board();
	while ((! solved()) && solveable)
	{
		cout << "stage " << j + 1 << endl;
		j++;
		for (i = 0; i < _options.size(); i++)
		{
			if (ops_sizes[i] != _options[i].size())
			{
				////////////////////////////////////////
				if ((j>4) || (_options[i].size() < 200))
				{
					if (i < _size[0])
					{
						if (show_2)
							cout << "similar in coll: " << i + 1 << ", size: " << _options[i].size() << endl;
					}
					else
					{
						if (show_2)
							cout << "similar in line: " << i + 1 - _size[0] << ", size: " << _options[i].size() << endl;
					}
					ops_sizes[i] = _options[i].size();
					line = similar(_options[i]);
					if (i < _size[0])
					{
						set_coll(line, i);
					}
					else
					{
						set_line(line, i - _size[0]);
					}
				}
			}
		}

		//////////////
		show_board();
		//////////////

		for (i = 0; i < _options.size(); i++)
		{
			if (_options[i].size() > 1)
			{
				if (i < _size[0])
				{
					if (show_2)
						cout << "filter in coll: " << i + 1 << ", size: " << _options[i].size();
					comp1 = get_coll(i);
					comp2 = get_coll(i, prev_board, _size[1]);
				}
				else
				{
					if (show_2)
						cout << "filter in line: " << i + 1 - _size[0] << ", size: " << _options[i].size();
					comp1 = get_line(i - _size[0]);
					comp2 = get_line(i - _size[0], prev_board, _size[0]);
				}
				if (comp1 != comp2)
				{
					_options[i] = filter(_options[i], comp1);
					cout << " -> " << _options[i].size() << endl;
				}
				else
				{
					cout << " -> SKIPPING" << endl;
				}
			}
		}
		cout << endl << previous_time() << endl;
		//////////////
		//show_board();
		//////////////
		if (prev_board == _board)
		{
			solveable = false;
		}
		else
		{
			prev_board = _board;
		}
	}

	cout << "time: " << solving_time() << endl;

	return solved();
}

string Nono::show_line(vector <int> line)
{
	string s = "";
	for (int i = 0; i < line.size(); i++)
	{
		switch (line[i])
		{
		case 0:
			s += " ";
			break;

		case 1:
			s += char(176);
			break;

		case 2:
			s += char(219);
			break;
		}
	}
	return s;
}

void Nono::show_board()
{
	int i;
	if (_show_stuff)
		cout << "size: " << _size[0] << " - " << _size[1] << endl;
	cout << char(201);
	for (i = 0; i < _size[0]; i++)
	{
		cout << char(205);
	}
	cout << char(187) << endl;
	for (i = 0; i < _size[1]; i++)
	{
		cout << char(186) << show_line(_board[i]) << char(186) << endl;
	}
	cout << char(200);
	for (i = 0; i < _size[0]; i++)
	{
		cout << char(205);
	}
	cout << char(188) << endl;
}

void Nono::show_board(vector <vector <int>> board)
{
	int i;
	if (_show_stuff)
		cout << "size: " << _size[0] << " - " << _size[1] << endl;
	cout << char(201);
	for (i = 0; i < board[0].size(); i++)
	{
		cout << char(205);
	}
	cout << char(187) << endl;
	for (i = 0; i < board.size(); i++)
	{
		cout << char(186) << show_line(board[i]) << char(186) << endl;
	}
	cout << char(200);
	for (i = 0; i < _size[0]; i++)
	{
		cout << char(205);
	}
	cout << char(188) << endl;
}

int Nono::movable(vector <int> params, int length)
{
	for (int i = 0; i < params.size(); i++)
	{
		length -= (params[i] + 1);
	}
	
	return length + 1;
}

unsigned Nono::nChoosek(unsigned n, unsigned k)
{
    if (k > n) return 0;
    if (k * 2 > n) k = n-k;
    if (k == 0) return 1;

    int result = n;
    for(int i = 2; i <= k; ++i )
	{
        result *= (n-i+1);
        result /= i;
    }
    return result;
}

unsigned Nono::combinum(unsigned n, unsigned k)
{
	return nChoosek((n + k - 1), (k - 1));
}

int Nono::sum(int* arr, int end, int start)
{
	int value = 0;
	for (int i = start; i < end; i++)
	{
		value += arr[i];
	}
	return value;
}

int Nono::sum(vector <int> arr, int end, int start)
{
	int value = 0;
	for (int i = start; i < end; i++)
	{
		value += arr[i];
	}
	return value;
}

vector <vector <int>> Nono::vars(int move, int size)
{
	vector <vector <int>> all;

	int i, j;
	vector <int> line;
	int tmp;
	int limit = combinum(move, size);
	int since = 0;
	
	line.push_back(move + 1);
	line.push_back(-1);
	for (i = 2; i < size; i++)
	{
		line.push_back(0);
	}

	for (i = 0; i < limit; i++)
	{
		if (line[0] > 0)
		{
			line[0]--;
			line[1]++;
		}
		else
		{
			for (j = 1; j < size - 1; j++)
			{
				tmp = sum(line, j + 1);
				if (tmp == line[j] && tmp != 0)
				{
					line[j + 1]++;
					line[j] = 0;
					line[0] = tmp - 1;
					break;
				}
			}
		}
		all.push_back(line);
	}

	return all;
}

vector <vector <int>> Nono::make_lines_vec(vector <vector <int>> vars, vector <int> params, int size)
{
	vector <vector <int>> lines;
	vector <int> tmp;
	int size_2 = vars.size();

	int i;

	for (i = 0; i < size_2; i++)
	{
		lines.push_back(create_line(params ,vars[i], size));
	}

	if (_show_stuff)
	{
		for (i = 0; i < lines.size(); i++)
		{
			cout << show_line(lines[i]) << endl;
		}
	}

	return lines;
}

vector <int> Nono::create_line(vector <int> params, vector <int> spaces, int size)
{
	vector <int> line;
	int i, j;
	int offset = 0;
	int size_2 = params.size();

	for (i = 0; i < size; i++)
	{
		line.push_back(0);
	}

	for (i = 0; i < size_2; i++)
	{
		offset += spaces[i];
		for (j = 0; j < params[i]; j++)
		{
			line[offset] = 2;
			offset++;
		}
		offset += 1;
	}

	return line;
}

void Nono::set_line(vector <int> line, int index)
{
	for (int i = 0; i < _size[0]; i++)
	{
		if (line[i] != 1)
		{
			_board[index][i] = line[i];
		}
	}
}

void Nono::set_coll(vector <int> line, int index)
{
	for (int i = 0; i < _size[1]; i++)
	{
		if (line[i] != 1)
		{
			_board[i][index] = line[i];
		}
	}
}

vector <int> Nono::get_line(int index)
{
	vector <int> line;
	for (int i = 0; i < _size[0]; i++)
	{
		line.push_back(_board[index][i]);
	}
	return line;
}

vector <int> Nono::get_coll(int index)
{
	vector <int> line;
	for (int i = 0; i < _size[1]; i++)
	{
		line.push_back(_board[i][index]);
	}
	return line;
}

vector <vector <int>> Nono::filter(vector <vector <int>> lines, vector <int> role)
{
	int i, j;
	int size = lines.size();
	for (i = 0; i < size; i++)
	{
		if (! same(lines[i], role))
		{
			lines.erase(lines.begin() + i);
			i--;
			size--;
		}
	}

	return lines;
}

bool Nono::same(vector <int> line, vector <int> role)
{
	int size = line.size();
	for (int i = 0; i < size; i++)
	{
		if ((role[i] != 1) && (line[i] != role[i]))
		{
			return false;
		}
	}
	return true;
}

vector <int> Nono::similar(vector <vector <int>> lines)
{
	int i, j;
	vector <int> line = lines[0];
	int size = line.size();
	int size_2 = lines.size();
	for (i = 0; i < size; i++)
	{
		for (j = 1; j < size_2; j++)
		{
			if (line[i] != lines[j][i])
			{
				line[i] = 1;
				break;
			}
		}
	}

	return line;
}

bool Nono::solved()
{
	int i, j;
	for (i = 0; i < _size[1]; i++)
	{
		for (j = 0; j < _size[0]; j++)
		{
			if (_board[i][j] == 1)
			{
				return false;
			}
		}
	}
	return true;
}

vector <int> Nono::get_line(int index, vector <vector <int>> board, int size)
{
	vector <int> line;
	for (int i = 0; i < size; i++)
	{
		line.push_back(board[index][i]);
	}
	return line;
}

vector <int> Nono::get_coll(int index, vector <vector <int>> board, int size)
{
	vector <int> line;
	for (int i = 0; i < size; i++)
	{
		line.push_back(board[i][index]);
	}
	return line;
}

string Nono::solving_time()
{
	time_t t = time(0);
	struct tm * tmp = localtime(&t);
	string ret;

	int b, c;

	b = tmp->tm_sec;
	b += tmp->tm_min * 60;
	b += tmp->tm_hour * 3600;

	c = b - _time_started;
		
	ret = to_string(c / 3600) + ":" + to_string((c / 60) % 60) + ":" + to_string(c % 60);

	return ret;
}

string Nono::previous_time()
{
	time_t t = time(0);
	struct tm * tmp = localtime(&t);
	string ret;

	int b, c;

	b = tmp->tm_sec;
	b += tmp->tm_min * 60;
	b += tmp->tm_hour * 3600;

	c = b - _time_previous;

	_time_previous = b;
		
	ret = to_string(c / 3600) + ":" + to_string((c / 60) % 60) + ":" + to_string(c % 60);

	return ret;
}

void Nono::write()
{
	ofstream f("solution.txt");
	int i, j;
	f.clear();
	if (solved())
	{
		cout << "writing" << endl;
		f << _size[0] << ", " << _size[1] << endl;
		for (i = 0; i < _size[1]; i++)
		{
			for (j = 0; j < _size[0]; j++)
			{
				f << _board[i][j];
				if (j != _size[0] - 1)
					f << ", ";
			}
			f << endl;
		}
	}
	f.close();
}