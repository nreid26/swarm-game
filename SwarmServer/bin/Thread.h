#ifndef _THREAD
#define _THREAD

#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <list>
#include <iostream>
#include <String>
#include <pthread.h>
#include "unistd.h"

#include "$.h"
#include "Exception.h"

template <class T> class Thread {
	//Statics
	private: static void* mainWrapper(void* v) { //Wrapper with correct signiture for pthread_create
		Thread* self = (Thread*)v;

		self->running = true;

		while(!self->cancelled) { self->result = self->run(); } //Repeat main as long as the thead is not cancelled (the run can call cancel)

		self->running = false;
		return NULL;
	}

	//Data
	private: pthread_t threadId;
	private: bool running;
	private: bool cancelled;
	private: $<T> result;

	//Constructor
	public: Thread() {}

	public: ~Thread() { //Execution cannot dangle, execution must be joined before it's container is destroyed
		cancel();
		join();
	}

	//Members
	public: void start() {
		if(running || cancelled) { throw new Exception("Thread was Started Previously"); }
		else if(pthread_create(&threadId, NULL, mainWrapper, this) != 0) { throw new Exception("Thread Could Not be Started"); }
	}

	protected: virtual $<T> run() = 0; //Must be overridden, return is set as result on the object

	public: void join() { void* toReturn; pthread_join(threadId, &toReturn); } //Wait for this thread to complete

	public: virtual void cancel() { cancelled = true;} //Ask this thread to stop

	public: bool isCancelled() { return cancelled; } //Has this thread been asked to stop?

	public: bool isRunning() { return running; } //Is the thread currently active?
	
	public: $<T> getResult() { return result; }
};

#endif // THREAD_H
