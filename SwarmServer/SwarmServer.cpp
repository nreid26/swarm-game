
#include "src/ConnectionDelegate.h"
#include <string>

#include <iostream>

using namespace std;

int main() {
	ConnectionDelegate del(6113);
	del.start();

	cout << "Server Started" << endl;

	string s = "";
	while(s != "exit") {
		getline(cin, s);
	}
}

