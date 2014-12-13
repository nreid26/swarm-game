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

Player::Player(WebSocket* connection) : connection(connection), id(nextId()), name(""), messenger(NULL) {
	start(); //Start player read thread
	cout << "New Player: " << id << endl;
}

Player::~Player() {
	delete connection;
	delete messenger;
}

const string& Player::getName() { return name; }

void Player::setName(string name) { 
	cout << "Named Player " << id << ": " << name << endl;
	this->name = name; 
}

int Player::getId() { return id; }

void Player::tell(string message) {
	try {
		connection->write(message); //Attempt to write
	}
	catch(Exception e) { //The socekt has crashed
		messenger->playerDied();
		cout << "Player " << id << " has died" << endl;
		cancel();
	}
}

void Player::setMessenger(Messenger* messenger) {
	delete this->messenger;
	this->messenger = messenger;
}

int* Player::run() {  //For all time, wait for a message and pass it to the messenger
	try {
		string s = connection->read();
		cout << "player " << id << " was told " << s << endl;
		messenger->tellWorld(s);
	}
	catch(Exception e) { //The socekt has crashed
		messenger->playerDied();
		cout << "Player " << id << " has died" << endl;
		cancel();
	}

	return NULL;
}