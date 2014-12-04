#ifndef _SEMAPHORE
#define _SEMAPHORE

#include <fcntl.h>           // For O_* constants
#include <sys/stat.h>        // For mode constants

#include "Exception.h"
#include "$.h"

class Semaphore {
	//Data
	private: sem_t sem;

	//Constructor
	public: Semaphore(int initialState = 0) {
		sem_init(&sem, 1, initialState); //1 indicates that the semaphore exists in shared memory

		if(&sem == SEM_FAILED) {
			throw new Exception("Couldn't create semaphore");
		}
	}

	public: virtual ~Semaphore() { sem_close(&sem); }

	public: void wait() { sem_wait(&sem); }

	public: void signal() { sem_post(&sem); }
};

typedef $<Semaphore> $Semaphore;

#endif
