#ifndef _CONNECTION_DELEGATE
#define _CONNECTION_DELEGATE

#include <iostream>

#include "Thread.h"
#include <string>
#include "Player.h"
#include "WebSocketServer.h"
#include "NamingMessenger.h"
#include "Semaphore.h"

#include <vector>

using namespace std;

class ConnectionDelegate : public Thread<void> {
	//Data
	private: WebSocketServer server;
	private: vector<Player*> players;
	private: bool dying;


	//Constructor
	public: ConnectionDelegate(int port) : server(port), dying(false) { }

	public: virtual ~ConnectionDelegate() {
		dying = true;
		for(int i = 0; i < players.size(); i++) { delete players[i]; }
	}

	protected: virtual void* run() {
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

	public: void removePlayer(Player* p) {
		if(dying) { return; } //Best I can do

		for(int i = 0; i < players.size(); i++) {
			if(players[i] == p) {
				players.erase(players.begin() + i);
			}
		}
	}
};

#endif