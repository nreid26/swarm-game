#ifndef _$
#define _$

#define NULL 0

#include <String>

using namespace std;

typedef string String;

template<class T> class $ {
	//Data
	private:int* users;
	private: T* obj;

	//Constructor
	public: $() { init(NULL); } //Default constructor

	public: $(T* ptr) { init(ptr); } //Allows for implicit conversion from pointer to Reference in function calls

	public: $(const $& origin) { duplicate(origin); } //Copy constructor

	//Specialized String literal constructor (allows String literal to substitute $<String> in function calls)
	public: template <size_t N> $<String>(const char (&origin)[N]) { $<String>(new String(origin)); } 

	public: ~$() { release(); }

	//Methods
	public: T* operator->() { return obj; } //Member access

	public: const $& operator=(const $& origin) { //Copy assignmnet (since any acceptable value can decay to $<T> through a construtors, this covers all cases)
		release();
		duplicate(origin);
		return *this;
	}

	public: template <class U> $<U> operator $<U>() {
		$<U> temp(&((U) this->*obj));
		
		delete temp.users;
		temp.users = users;
		(*users)++;

		return temp;
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

#endif