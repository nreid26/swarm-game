#ifndef _EXCEPTION
#define _EXCEPTION

#include <string>

using namespace std;

class Exception {
	//Data
	private:
	string message;

	//Constructor
	public: Exception(string message = "") : message(message) {}

	public: virtual ~Exception() {}
	//Members
	public: const string& getMessage() { return message; }
};

#endif