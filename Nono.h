#pragma once
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <ctime>

using namespace std;

class Nono
{
public:
	Nono();
	~Nono();
	bool solve(bool show = true, bool show_2 = false);
	string show_line(vector <int> line);
	void show_board();
	void show_board(vector <vector <int>> board);
	int movable(vector <int> params, int length);
	unsigned nChoosek(unsigned n, unsigned k);
	unsigned combinum(unsigned n, unsigned k);
	int sum(int* arr, int end, int start = 0);
	int sum(vector <int> arr, int end, int start = 0);
	vector <vector <int>> vars(int move, int size);
	vector <vector <int>> make_lines_vec(vector <vector <int>> vars, vector <int> params, int size);
	vector <int> create_line(vector <int> params, vector <int> spaces, int size);
	void set_line(vector <int> line, int index);
	void set_coll(vector <int> line, int index);
	vector <int> get_line(int index);
	vector <int> get_coll(int index);
	vector <vector <int>> filter(vector <vector <int>> lines, vector <int> role);
	bool same(vector <int> line, vector <int> role);
	vector <int> similar(vector <vector <int>> lines);
	bool solved();
	vector <int> get_line(int index, vector <vector <int>> board, int size);
	vector <int> get_coll(int index, vector <vector <int>> board, int size);
	string solving_time();
	string previous_time();
	void write();

	static vector <int> split(string s, string token);

private:
	vector <vector <int>> _board;
	vector <vector <int>> _params;
	vector <int> _size;
	vector <vector <vector <int>>> _options;
	bool _show_stuff;
	int _time_started;
	int _time_previous;
};