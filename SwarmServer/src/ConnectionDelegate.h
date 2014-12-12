#ifndef _CONNECTION_DELEGATE
#define _CONNECTION_DELEGATE

#include "Thread.h"
#include "WebSocketServer.h"

#include <vector>

class Player;

using namespace std;

class ConnectionDelegate : public Thread<int> {
	private: WebSocketServer server;
	private: vector<Player*> players;
	private: bool dying;

	public: ConnectionDelegate(int port);

	public: virtual ~ConnectionDelegate();
	protected: virtual int* run();
	public: void removePlayer(Player* p);
};

#endif