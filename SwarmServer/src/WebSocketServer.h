#ifndef _WEBSOCKET_SERVER
#define _WEBSOCKET_SERVER

#include "$.h"
#include "SocketServer.h"
#include "WebSocket.h"

#include <iostream>

#include "smallsha/sha1.h"

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
		cout << "New Connection";
		return result;
	}


	private: void prepare($WebSocket connection) {
		$String req = connection->read();
		$String res = "HTTP/1.1 101 Switching Protocols\r\nUpgrade: websocket\r\nConnection: Upgrade\r\nSec-WebSocket-Accept: ";

		appendHash(req, res); //Put hash in response
		res->append("\r\n\r\n"); //Terminate response

		connection->write(res); //Send response
	}

	private: void appendHash($String req, $String res) {
		const static char key[] = "Sec-WebSocket-Key: ";
		const static char keyEnd[] = "\r\n";
		const static char magicString[] = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";

		unsigned char hash[20]; //Magic size

		//Extract key locales from socket string
		int valStart = req->find(key) + sizeof(key) - 1;
		int valEnd = req->find(keyEnd, valStart);

		//Take hash of key + magic string
		$String s = new String(req->substr(valStart, valEnd - valStart));
		s->append(magicString);

		calc(s->c_str(), s->length(), hash);
		res->append(base64Encode(reinterpret_cast<const unsigned char*>(hash), 20)->c_str()); //Magic don't touch
	}

	//Not mine dont' touch
	$String base64Encode(unsigned char const* bytes_to_encode, unsigned int in_len) {
		static const char base64_chars[] = { "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/" };

		String ret = "";
		int i = 0;
		int j = 0;
		unsigned char char_array_3[3];
		unsigned char char_array_4[4];

		while(in_len--) {
			char_array_3[i++] = *(bytes_to_encode++);
			if(i == 3) {
				char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
				char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
				char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
				char_array_4[3] = char_array_3[2] & 0x3f;

				for(i = 0; i < 4; i++) { ret += base64_chars[char_array_4[i]]; }

				i = 0;
			}
		}

		if(i) {
			for(j = i; j < 3; j++) { char_array_3[j] = '\0'; }

			char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
			char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
			char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
			char_array_4[3] = char_array_3[2] & 0x3f;

			for(j = 0; (j < i + 1); j++) { ret += base64_chars[char_array_4[j]]; }
			while((i++ < 3)) { ret += '='; }
		}

		return new String(ret);

	}
};

typedef $<WebSocketServer> $WebSocketServer;

#endif