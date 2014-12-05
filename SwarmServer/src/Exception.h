#ifndef _EXCEPTION
#define _EXCEPTION

#include <string>
#include "$.h"

using namespace std;

class Exception {
	//Data
	private:
	$String message;

	//Constructor
	public: Exception($String message = (String*)NULL) : message(message) {}

	public: virtual ~Exception() {}
	//Members
	public: $String getMessage() { return message; }
};

typedef $<Exception> $Exception;

#endif