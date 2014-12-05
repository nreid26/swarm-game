#ifndef _WEBSOCKET
#define _WEBSOCKET

#include "$.h"
#include "Socket.h"

class WebSocket : public Socket {
	//Data

	//Constructor
	public: WebSocket($String ipAddress, unsigned int port) : Socket(ipAddress, port) { }

	public: WebSocket(int socketFile) : Socket(socketFile) { }

	//Methods
	public: virtual $String read() {
		return Socket::read();
	}
};

typedef $<WebSocket> $WebSocket;

#endif