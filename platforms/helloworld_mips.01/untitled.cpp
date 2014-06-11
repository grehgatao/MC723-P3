/* ANDRE NAKAGAKI FILLIETTAZ - RA104595 */

// INCLUDES
#include <string>
#include <iostream>
#include <fstream>
#include <cstdlib>

// PRAGMA
int max_value(int *);

// NAMESPACES
using namespace std;

int main (int argc, char *argv[]) {
	if (argc < 2)	return -1;

	/* declaracoes */
	ifstream infile(argv[1]);
	char tmp[256]="lixo";
	int i, test[8];

	for (i=0; i<8; i++) {
		infile.getline(tmp, 256);
		cout << tmp << endl;
		test[i]=atoi(tmp);
		cout << test[i] << endl;
	}

	cout << max_value(test) << endl;

	return 0;
}

int max_value(int *array) {
	int max=array[0], i;
	
	for (i=1; i<8; i++)
		max = (max<array[i]) ? array[i] : max;
	return max;
}