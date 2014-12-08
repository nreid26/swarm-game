#ifndef _WEBSOCKET_SERVER
#define _WEBSOCKET_SERVER

#include "$.h"
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
	public: $WebSocket accept() {
		if(isClosed()) { throw new Exception("Accept Cannot be Called on a Closed Socket"); }

		int connectionFile = ::accept(socketFile, NULL, 0);
		if(connectionFile < 0) { throw new Exception("Unexpected Error in the Server"); }

		$WebSocket result = new WebSocket(connectionFile);
		cout << "New Connection" << endl;
		return result;
	}
};

typedef $<WebSocketServer> $WebSocketServer;

#endif