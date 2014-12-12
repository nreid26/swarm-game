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

ConnectionDelegate::ConnectionDelegate(int port) : server(port), dying(false) { }

ConnectionDelegate::~ConnectionDelegate() {
	dying = true;
	for(int i = 0; i < players.size(); i++) { delete players[i]; }
}

int* ConnectionDelegate::run() {
		try {
			WebSocket* connection = server.accept();

			Player* player = new Player(connection, this);
			players.push_back(player);
			player->setMessenger(new NamingMessenger(player));
		}
		catch(Exception e) {
			cerr << e.getMessage() << endl;
		}

		return NULL;
	}	

void ConnectionDelegate::removePlayer(Player* p) {
	if(dying) { return; } //Best I can do

	for(int i = 0; i < players.size(); i++) {
		if(players[i] == p) {
			players.erase(players.begin() + i);
		}
	}
}