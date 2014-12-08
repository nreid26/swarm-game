#ifndef _WEBSOCKET
#define _WEBSOCKET

#include "$.h"
#include "Socket.h"
#include "SHA1.h"

#include <iostream>

using namespace std;

class WebSocket : public Socket {
	//Data

	//Constructor
	public: WebSocket(int socketFile) : Socket(socketFile) { prepare(); }

	public: virtual ~WebSocket() { }

	//Methods
	public: $String read() {
		//Many magic numbers from MDN spec

		//Extact payload size
		int payloadSize = Socket::read(2)->at(1) & 0x7F; //Set upper MSB to 0
		if(payloadSize == 126) { payloadSize = extract(Socket::read(2)); }
		else if(payloadSize == 127) { payloadSize = extract(Socket::read(4)); }

		$String mask = Socket::read(4); //Read XOR mask
		$String data = Socket::read(payloadSize); //Read in data

		for(int i = 0; i < data->length(); i++) {
			data->at(i) ^= mask->at(i % 4); //Apply mask
		}

		return data;
	}


	private: int extract($String header) {
		int ret = 0;

		for(int i = 0; i < header->length(); i++) {
			ret <<= 8;
			ret += (unsigned char)(header->at(i));
		}

		return ret;
	}

	private: void prepare() {
		$String req = Socket::read();
		$String res = "HTTP/1.1 101 Switching Protocols\r\nUpgrade: websocket\r\nConnection: Upgrade\r\nSec-WebSocket-Accept: ";

		appendHash(req, res); //Put hash in response
		res->append("\r\n\r\n"); //Terminate response

		write(res); //Send response
	}

	private: void appendHash($String req, $String res) {
		const static char key[] = "Sec-WebSocket-Key: ";
		const static char keyEnd[] = "\r\n";
		const static char magicString[] = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";

		//Extract key locales from socket string
		int valStart = req->find(key) + sizeof(key)-1;
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

typedef $<WebSocket> $WebSocket;

#endif