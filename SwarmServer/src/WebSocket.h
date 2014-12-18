#ifndef _WEBSOCKET
#define _WEBSOCKET

#include "Socket.h"
#include "SHA1.h"

#include <iostream>
#include "Exception.h"

using namespace std;

class WebSocket : public Socket {
	//Data

	//Constructor
	public: WebSocket(int socketFile) : Socket(socketFile) { prepare(); }

	public: virtual ~WebSocket() { }

	//Methods
	public: virtual string read() {
		//Check for closing
		int x = Socket::read(1)[0] & 0xFF;
		if(x == 0x88) {
			throw Exception("WebSocket Has Been Disconnected From Client Side");
		}

		//Extact payload size
		int payloadSize = Socket::read(1)[0] & 0x7F; //Set upper MSB to 0
		if(payloadSize == 126) { payloadSize = extract(Socket::read(2)); }
		else if(payloadSize == 127) { payloadSize = extract(Socket::read(4)); }

		string mask = Socket::read(4); //Read XOR mask
		string data = Socket::read(payloadSize); //Read in data

		for(int i = 0; i < data.length(); i++) {
			data[i] ^= mask[i % 4]; //Apply mask
		}

		return data;
	}

	public: virtual int write(string& message) {
		static const char magicByte = 128 + 1; //First byte of message, FIN=1 OPCODE=0x1

		string toSend(&magicByte); //What the socket will receive

		//MASK is bit 8 of first size bit; size bit must be < 128
		if(message.size() <= 125) { toSend.push_back( (char)message.size() ); } //Push back first byte which is size
		else if(message.size() < (1 << 16)) { 
			toSend.push_back(126); //Puch back first byte, then size as next two bytes
			for(int i = 1; i >= 0; i--) { 
				toSend.push_back( (char)(message.size() >> (8 * i)) );
			}
		}
		else { 
			toSend.push_back(126); //Puch back first byte, then size as next four bytes
			for(int i = 3; i >= 0; i--) {
				toSend.push_back( (char)(message.size() >> (8 * i)) );
			}
		}

		toSend.append("\0\0\0\0"); //Empty mask bytes
		toSend.append(message);

		return Socket::write(toSend); //Call to base function
	}


	private: int extract(string header) {
		int ret = 0;

		for(int i = 0; i < header.length(); i++) {
			ret <<= 8;
			ret += (unsigned char)(header.at(i));
		}

		return ret;
	}

	private: void prepare() {
		string req = Socket::read();
		string res("HTTP/1.1 101 Switching Protocols\r\nUpgrade: websocket\r\nConnection: Upgrade\r\nSec-WebSocket-Accept: ");

		appendHash(req, res); //Put hash in response
		res.append("\r\n\r\n"); //Terminate response

		write(res); //Send response
	}

	private: void appendHash(string& req, string& res) {
		const static string key("Sec-WebSocket-Key: ");
		const static string keyEnd("\r\n");
		const static string magicString("258EAFA5-E914-47DA-95CA-C5AB0DC85B11");

		//Extract key locales from socket string
		int valStart = req.find(key) + key.length();
		int valEnd = req.find(keyEnd, valStart);

		//Take hash of key + magic string
		string s(req.substr(valStart, valEnd - valStart));
		s.append(magicString);

		res.append(
			SHA1::toBase64string(
				SHA1::calc(s)
			)
		);
	}
};

#endif