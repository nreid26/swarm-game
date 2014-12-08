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
	public: virtual $String read(int len = 0) {
		//Many magic numbers from MDN spec

		$String header = Socket::read(10); //Read length headers
		$String mask = Socket::read(4); //Read XOR mask

		//Extact payload size
		int payloadSize = header->at(1) & 0x7F;
		if(payloadSize == 126) { payloadSize = extract(header, 2, 4); }
		else if(payloadSize == 127) { payloadSize = extract(header, 4, 8); }

		$String data = Socket::read(payloadSize); //Read in data

		for(int i = 0; i < data->length(); i++) {
			data->at(i) ^= mask->at(i % 4); //Apply mask
		}

		return data;
	}

	private: int extract($String header, int start, int end) {
		int ret = 0;

		while(start < end) {
			ret <<= 8;
			ret += (unsigned char)(header->at(start));
		}

		return ret;
	}
};

typedef $<WebSocket> $WebSocket;

#endif