#ifndef _THREAD_DISPOSER
#define _THREAD_DISPOSER

#include "Thread.h"
#include "Semaphore.h"
#include <vector>

#include <unistd.h> //Sleep

class ThreadDisposer : private Thread<int> {
	//Statics
	public: static ThreadDisposer& getInstance() {
		static ThreadDisposer instance;
		return instance;
	}

	//Constructor
	private: ThreadDisposer() : guard(1) {
		start();
	}

	public: virtual ~ThreadDisposer() { }

	//Data
	private: vector<ThreadBase*> toKill;
	private: Semaphore guard;

	//Methods
	protected: virtual int* run() {
		usleep(1000 * 1000);

		guard.wait();
			for(int i = toKill.size() - 1; i >= 0; i--) { //Iterate over to kill and erase any threads that are not running
				if(!toKill[i]->isRunning()) {
					delete toKill[i];
					toKill.erase(toKill.begin() + 1);
				}
			}
		guard.signal();

		return NULL;
	}

	public: void add(ThreadBase* victim) {
		guard.wait();
			for(int i = 0; i < toKill.size(); i++) { //Only add a victim once
				if(toKill[i] == victim) {
					guard.signal();
					return;
				}
			}

			toKill.push_back(victim);
		guard.signal();
	}
};

#endif