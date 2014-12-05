#ifndef _PLAYER
#define _PLAYER

#include <string>

#include "Thread.h"
#include "Socket.h"
#include "Semaphore.h"
#include "$.h"

using namespace std;

class Player : public Thread<void> {
	//Statics
	public: static int nextId() { 
		static int statId = 0;
		static $Semaphore guard = new Semaphore(1);

		guard->wait();
			int result = statId++;
		guard->signal();
		return result;
	}

	//Data
	private: int id;
	private: $Messenger messenger;
	private: $String name;
	private: $Socket connection;

	//Constructor
	public: Player($Socket connection) : connection(connection), id(nextId()) { }

	public: virtual ~Player() {}

	//Methods
	public: $String getName() { return name; }

	public: void setName($String name) { this->name = name; }

	public: int getId() { return id; }

	public: void tell($String message) { connection->write(message); }

	public: void setMessenger($Messenger messenger) { this->messenger = messenger; }

	protected: virtual $<void> run() { messenger->tellWorld(connection->read()); } //For all time, wait for a message and pass it to the messenger
};

typedef $<Player> $Player;

#endif