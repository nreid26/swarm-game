#include "Player.h"

#include <string>

#include "Thread.h"
#include "Semaphore.h"
#include "ThreadDisposer.h"
#include "WebSocket.h"
#include "ConnectionDelegate.h"
#include "Messenger.h"

using namespace std;

int Player::nextId() { 
	static int statId = 0;
	static Semaphore guard(1);

	guard.wait();
		int result = statId++;
	guard.signal();
	return result;
}

Player::Player(WebSocket* connection, ConnectionDelegate* del) : connection(connection), id(nextId()), name(""), del(del) {
	start(); //Start player read thread
}

Player::~Player() {
	delete connection;
	delete messenger;
}

const string& Player::getName() { return name; }

void Player::setName(string name) { this->name = name; }

int Player::getId() { return id; }

void Player::tell(string message) { connection->write(message); }

void Player::setMessenger(Messenger* messenger) {
	delete this->messenger;
	this->messenger = messenger;
}

int* Player::run() {  //For all time, wait for a message and pass it to the messenger
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