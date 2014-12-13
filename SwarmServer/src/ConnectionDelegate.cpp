#include "ConnectionDelegate.h"

#include "Thread.h"
#include "Player.h"
#include "WebSocketServer.h"
#include "NamingMessenger.h"
#include "Semaphore.h"

#include <iostream>
#include <string>
#include <vector>

using namespace std;

ConnectionDelegate::ConnectionDelegate(int port) : server(port) { }

ConnectionDelegate::~ConnectionDelegate() {
	for(int i = 0; i < players.size(); i++) { delete players[i]; }
}

int* ConnectionDelegate::run() {
	try {
		WebSocket* connection = server.accept();

		Player* player = new Player(connection);

		players.push_back(player);
		player->setMessenger(new NamingMessenger(player));
	}
	catch(Exception e) {
		cerr << e.getMessage() << endl;
	}

	return NULL;
}	
