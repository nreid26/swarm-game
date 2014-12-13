
#include "src/ConnectionDelegate.h"
#include <string>

#include <iostream>

using namespace std;

int main() {
	try {
		ConnectionDelegate del(6113);
		del.start();

		cout << "Server Started" << endl;

		string s = "";
		while(s != "exit") {
			getline(cin, s);
		}
	}
	catch(Exception e) {
		cerr << "Server Failed: " << e.getMessage() << endl;
	}
}

