#ifndef _CONNECTION_DELEGATE
#define _CONNECTION_DELEGATE

#include <iostream>

#include "$.h"
#include "Thread.h"
#include <string>
#include "Player.h"
#include "WebSocketServer.h"
#include "NamingMessenger.h"

using namespace std;

class ConnectionDelegate : public Thread<void> {
	//Data
	private: $WebSocketServer server;

	//Constructor
	public: ConnectionDelegate(int port) : server(new WebSocketServer(port)) { }

	protected: virtual void main() {
		try {
			$WebSocket connection = server->accept();

			$Player player = new Player(connection);
			player->start(); //Start player read thread
			player->setMessenger(new NamingMessenger(player));
		}
		catch(Exception e) {
			cerr << e.getMessage()->c_str() << endl;
		}
	}	
};

typedef $<ConnectionDelegate> $ConnectionDelegate;

#endif