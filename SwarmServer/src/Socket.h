#ifndef _Socket
#define _Socket

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>

#include <unistd.h>
#include <iostream>
#include <errno.h>
#include <strings.h>

#include <string>
#include "Semaphore.h"

using namespace std;

class Socket {
	//Statics
	const static int bufferSize = 1024;

	//Data
	private: sockaddr_in socketDescriptor;
	private: int socketFile;
	private: bool closed;

	private: char buffer[bufferSize]; //Read buffer

	protected: Semaphore readGuard;
	protected: Semaphore writeGuard;

	//Constructor
	public: Socket(string& ipAddress, unsigned int port) : closed(false), readGuard(1), writeGuard(1) {
		//Open socket file
		if(socketFile = socket(AF_INET, SOCK_STREAM, 0) < 0) {
			throw Exception("Unable to Initialize");
		}

		bzero((char*)&socketDescriptor, sizeof(sockaddr_in)); //0 socket descriptor

		//Bind IP
		if(!inet_aton(ipAddress.c_str(), &socketDescriptor.sin_addr)) {
			throw Exception("Invalid IP Address");
		}

		socketDescriptor.sin_family = AF_INET;
		socketDescriptor.sin_port = htons(port);
	}

	public: Socket(int socketFile) : closed(false), socketFile(socketFile), readGuard(1), writeGuard(1) { }

	public: virtual ~Socket() {
		close();
	}

	//Methods
	public: int open() {
		if(connect(socketFile, (sockaddr*)&socketDescriptor, sizeof(socketDescriptor)) != 0)
		{
			throw Exception("Unable to Open Connection");
		}
	}

	public: int write(string& message) {
		if(closed) { throw Exception("Attempt to write to an unopened socket"); }

		//1 writer
		writeGuard.wait();
		try {
			int result = ::write(socketFile, message.c_str(), message.size());
			writeGuard.signal();
			return result;
		}
		catch(...) { //If write fails, signal sem and throw
			writeGuard.signal();
			throw Exception("Socket Shutdown During Write");
		}
	}

	public: string read(int len = 0) {
		if(closed) { throw Exception("Cannot Read an Unopened Socket"); }

		if(len <= 0 || len > bufferSize) { len = bufferSize; } //Unspecified -> max

		//1 reader
		readGuard.wait(); 
		try {
			size_t received = recv(socketFile, buffer, len, 0); //Block to read, get amount
			string result(buffer, received);
			readGuard.signal(); //Release 
			return result; //Return the string
		}
		catch(...) { //If read fails, signal sem and throw
			readGuard.signal();  
			throw Exception("Socket Shutdown During Read"); 
		}
	}

	public: void close() {
		if(!closed) {
			shutdown(socketFile, SHUT_RDWR);
			::close(socketFile);
			closed = true;
		}
	}

	public: bool isClosed() { return closed; }
};

#endif