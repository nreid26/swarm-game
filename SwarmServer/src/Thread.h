#ifndef _THREAD
#define _THREAD

#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <iostream>
#include <string>
#include <pthread.h>
#include "unistd.h"

#include "Exception.h"

#include <string>

class ThreadBase {
	//Data
	protected: pthread_t threadId;
	protected: bool running;
	protected: bool cancelled;
			   
	//Constructor
	public: ThreadBase() : running(false), cancelled(false) { }

	public: virtual ~ThreadBase() { //Execution cannot dangle, execution must be joined before it's container is destroyed
		cancel();
		join();
	}

	//Methods
	public: void join() { void* toReturn; pthread_join(threadId, &toReturn); } //Wait for this thread to complete

	public: virtual void cancel() { cancelled = true; } //Ask this thread to stop

	public: bool isCancelled() { return cancelled; } //Has this thread been asked to stop?

	public: bool isRunning() { return running; } //Is the thread currently active?
};


template <class T> class Thread : public ThreadBase {
	//Statics
	private: static void* mainWrapper(void* v) { //Wrapper with correct signiture for pthread_create
		Thread* self = (Thread*)v;

		self->running = true;

		try {
			while(!self->cancelled) { self->result = self->run(); } //Repeat main as long as the thead is not cancelled (the run can call cancel)
		}
		catch(Exception e) {
			cerr << "Thread Terminated Unexpectedly: " << e.getMessage() << endl;
		}

		self->running = false;
		return NULL;
	}

	//Data
	private: T* result;

	//Constructor
	public: Thread() { }

	public: virtual ~Thread() {
		delete result;
	}

	//Members
	public: void start() {
		if(running || cancelled) { throw Exception("Thread was Started Previously"); }
		else if(pthread_create(&threadId, NULL, mainWrapper, this) != 0) { throw Exception("Thread Could Not be Started"); }
	}

	protected: virtual T* run() = 0; //Must be overridden, return is set as result on the object
	
	public: T* getResult() { return result; }
};

#endif // THREAD_H
