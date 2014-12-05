#ifndef _String_BUILDER
#define _String_BUILDER

#include "$.h"

using namespace std;

class StringBuilder {
	//Building linked list node
	private: class Car {
		//Data
		public: $<String> part;
		public: $<Car> next;
	};

	//Data
	private: $<Car> first;
	private: $<Car> last;

	//Constructor
	public: StringBuilder() : first(new Car()) {
		last = first;
	}

	//Methods
	public: int size() { //Get the size of the resulting String
		int result = 0;
		$<Car> current = first;

		while(current->part != NULL) {
			result += current->part->size();
			current = current->next;
		}

		return result;
	}

	public: $<String> get() { //Get the result of the builder
		char* buffer = new char[size()];
		int bufferIndex = 0;

		$<Car> current = first;
		while(current->part != NULL) {
			for(int i = 0; i < current->part->size();) {
				buffer[bufferIndex++] = current->part->at(i++);
			}
			current = current->next;
		}

		$<String> result(new String(buffer, bufferIndex));
		delete[] buffer;
		return result;
	}

	public: void add($<String> str) { //Add a new String to the end
		last->part = str;
		last->next = new Car();
		last = last->next;
	}
};

#endif