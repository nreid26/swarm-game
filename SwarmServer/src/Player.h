#ifndef _PLAYER
#define _PLAYER

#include <string>

#include "Thread.h"
#include "Semaphore.h"
#include "ThreadDisposer.h"
#include "WebSocket.h"
#include "ConnectionDelegate.h"
#include "Messenger.h"

class ConnectionDelegate;

using namespace std;

class Player : public Thread<void> {
	public: static int nextId() { 
		static int statId = 0;
		static Semaphore guard(1);

		guard.wait();
			int result = statId++;
		guard.signal();
		return result;
	}

	//Data
	private: int id;
	private: Messenger* messenger;
	private: string name;
	private: WebSocket* connection;
	private: ConnectionDelegate* del;

	//Constructor
	public: Player(WebSocket* connection, ConnectionDelegate* del) : connection(connection), id(nextId()), name(""), del(del) {
		start(); //Start player read thread
	}

	public: virtual ~Player() {
		delete connection;
		delete messenger;
	}

	//Methods
	public: string getName() { return name; }

	public: void setName(string name) { this->name = name; }

	public: int getId() { return id; }

	public: void tell(string message) { connection->write(message); }

	public: void setMessenger(Messenger* messenger) { 
		delete this->messenger;
		this->messenger = messenger;
	}

	protected: virtual void* run() {  //For all time, wait for a message and pass it to the messenger
		try {
			messenger->tellWorld(connection->read());
		}
		catch(Exception e) { //The socekt has crashed
			messenger->playerDied();
			del->removePlayer(this);

			//Schedule myself to die
			ThreadDisposer::getInstance().add(this);
			cancel();
		}

		return NULL;
	}
};

#endif