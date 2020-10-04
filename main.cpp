#include "Nono.h"

/*
weak composition
(n + k - 1) C (k - 1)
( (n + k - 1) )
(   (k - 1)   )

n - move
k - size
*/

int main()
{
	Nono n;

	//get everything back to int!!!
	//boleans save memory but are very slow... :(

	system("mode con cols=80 lines=5000");

	bool solved;
	solved = n.solve(false, true);

	if (solved)
	{
		cout << "SOLVED!" << endl;
		n.show_board();
		n.write();
	}
	else
	{
		cout << "there's probably more then one solotion, stop messing around" << endl;
	}

	system("pause");

	return 0;
}