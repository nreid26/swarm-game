#ifndef _WEBSOCKET_SERVER
#define _WEBSOCKET_SERVER

#include "SocketServer.h"
#include "WebSocket.h"

#include <iostream>

using namespace std;

class WebSocketServer : public SocketServer {
	//Data

	//Constructor
	public: WebSocketServer(int port) : SocketServer(port) { }

	public: virtual ~WebSocketServer() { }

	//Methods
	public: virtual WebSocket* accept() {
		if(isClosed()) { throw Exception("Accept Cannot be Called on a Closed Socket"); }

		int connectionFile = ::accept(socketFile, NULL, 0);
		if(connectionFile < 0) { throw Exception("WebSocket Server Shutdown"); }

		return new WebSocket(connectionFile);
	}
};

#endif