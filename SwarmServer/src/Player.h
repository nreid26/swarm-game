#ifndef _PLAYER
#define _PLAYER

#include <string>

#include "Thread.h"
#include "Socket.h"
#include "Semaphore.h"
#include "ThreadDisposer.h"
#include "WebSocket.h"
#include "$.h"

using namespace std;

class Player : public Thread<void> {
	//Statics
	public: static $<Player> createPlayer($WebSocket connection) {
		$<Player> p = new Player(connection);
		p->myself = p;
		return p;
	}

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
	private: $WebSocket connection;
	private: $<Player> myself;

	//Constructor
	private: Player($WebSocket connection) : connection(connection), id(nextId()), name("") { }

	public: virtual ~Player() {}

	//Methods
	public: $String getName() { return name; }

	public: void setName($String name) { this->name = name; }

	public: int getId() { return id; }

	public: void tell($String message) { connection->write(message); }

	public: void setMessenger($Messenger messenger) { this->messenger = messenger; }

	protected: virtual $<void> run() {  //For all time, wait for a message and pass it to the messenger
		try {
			messenger->tellWorld(connection->read());
		}
		catch(Exception e) { //The socekt has crashed
			messenger->playerDied();
			messenger = NULL;

			//Schedule myself to die
			ThreadDisposer::getInstance()->add(myself);
			myself = NULL;
			cancel();
		}

		return NULL;
	}
};

typedef $<Player> $Player;

#endif