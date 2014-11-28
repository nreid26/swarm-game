#ifndef _EXCEPTION
#define _EXCEPTION

#include <String>
#include "../$.h"

using namespace std;

class Exception {
	//Data
	private:
	$<String> message;
	$<void> object;

	//Constructor
	public: Exception($<String> message = NULL, $<void> object = NULL) : message(message), object(object) {}

	public: virtual ~Exception() {}
	//Members
	public: $<String> getMessage() { return message; }

	public: $<void> getObject() { return object; }
};

#endif