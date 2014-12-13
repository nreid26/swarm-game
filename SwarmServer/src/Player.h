#ifndef _PLAYER
#define _PLAYER

#include <string>

#include "Thread.h"
#include "Semaphore.h"
#include "ThreadDisposer.h"
#include "WebSocket.h"

class Messenger;

using namespace std;

class Player : public Thread<int> {
	private: static int nextId();

	private: const int id;
	private: Messenger* messenger;
	private: string name;
	private: WebSocket* connection;

	public: Player(WebSocket* connection);
	public: virtual ~Player();

	public: const string& getName();
	public: void setName(string name);
	public: int getId();
	public: void tell(string message);
	public: void setMessenger(Messenger* messenger);
	protected: virtual int* run();
};

#endif