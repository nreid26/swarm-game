#ifndef _THREAD_DISPOSER
#define _THREAD_DISPOSER

#include "$.h"
#include "Thread.h"
#include "Semaphore.h"
#include <vector>

#include <unistd.h> //Sleep

class ThreadDisposer : private Thread<void> {
	//Statics
	public: static $<ThreadDisposer> getInstance() {
		static $<ThreadDisposer> instance = new ThreadDisposer();
		return instance;
	}

	//Constructor
	private: ThreadDisposer() : guard(1) {
		this->start();
	}

	//Data
	private: vector<$ThreadBase> toKill;
	private: Semaphore guard;

	//Methods
	protected: virtual $<void> run() {
		guard.wait();
			toKill.clear();
		guard.signal();

		usleep(1000 * 1000);

		return NULL;
	}

	public: void add($ThreadBase victim) {
		guard.wait();
			toKill.push_back(victim);
		guard.signal();
	}
};

#endif