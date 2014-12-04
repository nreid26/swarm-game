#ifndef _WEBSOCKET_SERVER
#define _WEBSOCKET_SERVER

#include "$.h"
#include "SocketServer.h"
#include "WebSocket.h"


#include "smallshaw/sha1.h"

using namespace std;
using namespace sha1;

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
		return result;
	}


	private: void prepare($Socket connection) {
		$String req = connection->read();
		$String res = "HTTP / 1.1 101 Switching Protocols\r\nUpgrade: websocket\r\nConnection: Upgrade\r\nSec-WebSocket-Accept: ";

		appendHash(req, res); //Put hash in response
		res->append("\r\n\r\n"); //Terminate response

		connection->write(res); //Send response
	}

	private: void appendHash($String req, $String res) {
		const static char key[] = { "Sec-WebSocket-Key: " };
		const static char keyEnd[] = { "\r\n" };

		unsigned char hash[20]; //Magic size
		char hexstring[41]; //Magic size

		//Extract key locales from socket string
		int valStart = req->find(key) + sizeof(key);
		int valEnd = req->find(keyEnd, valStart);

		//Take hash
		calc(req->substr(valStart, valEnd).c_str(), valEnd - valStart, hash);
		toHexString(hash, hexstring);

		res->append(hexstring); //Append
	}
};

typedef $<WebSocketServer> $WebSocketServer;

#endif