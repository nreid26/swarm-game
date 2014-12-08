#ifndef _$
#define _$

#define NULL 0

#include <string>
#include <iostream>

using namespace std;

typedef string String;

template<class T> class $ {
	//Data
	private: int* users;
	private: T* obj;

	//Constructor
	public: $() { init(NULL); } //Default constructor

	public: $(T* ptr) {
		init(ptr);
	} //Allows for implicit conversion from pointer to Reference in function calls

	public: $(const $& origin) {
		duplicate(origin);
	} //Copy constructor

	public: ~$() { release(); }

	//Methods
	public: T* operator->() { return obj; } //Member access

	public: const $& operator=(const $& origin) { //Copy assignmnet (since any acceptable value can decay to $<T> through a construtors, this covers all cases)
		release();
		duplicate(origin);
		return *this;
	}

	public: template<typename> friend class $;

	public: template <class U> operator $<U>() {
		cout << "casting" << endl;
		$<U> temp = $<U>((U*)(this->obj));

		delete temp.users;
		temp.users = users;
		(*users)++;
	}


	public: bool operator==(const $& other) { return obj == other.obj; } //Equality

	public: bool operator!=(const $& other) { return !(*this == other); } //Inequality

	//Base functions for many others
	private: void init(T* ptr) {
		users = new int(1);
		obj = ptr;
	}

	private: void duplicate(const $& origin) {
		users = origin.users;
		(*users)++;

		obj = origin.obj; //The address of the object in origin cast to typeof(this)
	}

	private: void release() {
		(*users)--;

		if(*users == 0) {
			delete obj;
			delete users;
		}
	}
};

class $String: public $<String> {
	public: $String(const char* origin) : $<String>(new String(origin)) { }
	public: $String(String* origin) : $<String>(origin) { }
};


#endif