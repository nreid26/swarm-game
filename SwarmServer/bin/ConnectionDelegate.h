#ifndef _CONNECTION_DELEGATE
#define _CONNECTION_DELEGATE

#include <iostream>

#include "$.h"
#include "Thread.h"
#include "Exception.h"
#include "Player.h"
#include "WebSocketServer.h"
#include "NamingMessenger.h"

using namespace std;

class ConnectionDelegate : public Thread<void> {
	//Data
	private: $WebSocketServer server;

	//Constructor
	public: ConnectionDelegate(int port) : server(new SocketServer(port)) { }

	protected: virtual void main() {
		try {
			$Socket connection = server->accept();

			prepare(connection); //Respond to the http request

			$Player player = new Player(connection);
			player->start(); //Start player read thread
			player->setMessenger(new NamingMessenger(player));
		}
		catch($Exception e) {
			cerr << e->getMessage()->c_str() << endl;
		}
	}	
};

typedef $<ConnectionDelegate> $ConnectionDelegate;

#endif