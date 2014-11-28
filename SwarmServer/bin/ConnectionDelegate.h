#ifndef _CONNECTION_DELEGATE
#define _CONNECTION_DELEGATE

#include "$.h"
#include "Thread.h"
#include "Exception.h"
#include "Player.h"
#include "SocketServer.h"
#include "Lobby.h"
#include "LobbyMessenger.h"

using namespace std;

class ConnectionDelegate : public Thread<void> {
	//Data
	private: $<Lobby> lobby;
};

#endif