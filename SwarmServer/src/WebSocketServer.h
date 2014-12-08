#ifndef _WEBSOCKET_SERVER
#define _WEBSOCKET_SERVER

#include "$.h"
#include "SocketServer.h"
#include "WebSocket.h"

#include <iostream>

#include "SHA1.h"

using namespace std;

class WebSocketServer : public SocketServer {
	//Data

	//Constructor
	public: WebSocketServer(int port) : SocketServer(port) { }

	public: virtual ~WebSocketServer() { }

	//Methods
	public: virtual $Socket accept() {
		if(isClosed()) { throw new Exception("Accept Cannot be Called on a Closed Socket"); }

		int connectionFile = ::accept(socketFile, NULL, 0);
		if(connectionFile < 0) { throw new Exception("Unexpected Error in the Server"); }

		$WebSocket result = new WebSocket(connectionFile);

		prepare(result);
		cout << "New Connection" << endl;
		return result;
	}


	private: void prepare($WebSocket connection) {
		$String req = connection->read();
		$String res = "HTTP/1.1 101 Switching Protocols\r\nUpgrade: websocket\r\nConnection: Upgrade\r\nSec-WebSocket-Accept: ";

		appendHash(req, res); //Put hash in response
		res->append("\r\n\r\n"); //Terminate response

		cout << res->c_str() << endl;

		connection->write(res); //Send response
	}

	private: void appendHash($String req, $String res) {
		const static char key[] = "Sec-WebSocket-Key: ";
		const static char keyEnd[] = "\r\n";
		const static char magicString[] = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";

		//Extract key locales from socket string
		int valStart = req->find(key) + sizeof(key) - 1;
		int valEnd = req->find(keyEnd, valStart);

		//Take hash of key + magic string
		$String s = new String(req->substr(valStart, valEnd - valStart));
		s->append(magicString);

		res->append(
			SHA1::toBase64String(
				SHA1::calc(s)
			)->c_str()
		);
	}	
};

typedef $<WebSocketServer> $WebSocketServer;

#endif