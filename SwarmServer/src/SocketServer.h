#ifndef _SOCKET_SERVER
#define _SOCKET_SERVER

#include "Socket.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h> 

#include "Exception.h"

#include <string>

using namespace std;

class SocketServer {
	//Data
	protected: int socketFile;
	private: sockaddr_in socketDescriptor;
	private: bool closed;

	//Constructor
	public: SocketServer(int port) {
		// The first call has to be to socket(). This creates a UNIX socket.
		socketFile = socket(AF_INET, SOCK_STREAM, 0);
		if(socketFile < 0)
			throw Exception("Unable to open the socket server");

		// The second call is to bind().  This identifies the socket file
		// descriptor with the description of the kind of socket we want to have.
		bzero((char*)&socketDescriptor, sizeof(sockaddr_in));
		socketDescriptor.sin_family = AF_INET;
		socketDescriptor.sin_port = htons(port);
		socketDescriptor.sin_addr.s_addr = INADDR_ANY;

		if(bind(socketFile, (sockaddr*)&socketDescriptor, sizeof(socketDescriptor)) < 0) {
			throw Exception("Unable to bind socket to requested port");
		}

		// Set up a maximum number of pending connections to accept
		listen(socketFile, 5);
	}

	public: virtual ~SocketServer() {
		close();
	}

	//Methods
	public: virtual Socket* accept() { //Blocks on the bound port and produces new Sockets
		if(closed) { throw Exception("Accept Cannot be Called on a Closed Socket"); }

		int connectionFile = ::accept(socketFile, NULL, 0);
		if(connectionFile < 0) { throw Exception("Unexpected Error in the Server"); }

		return new Socket(connectionFile);
	}

	public: void close() { //Close the socket and force an calls to accept to fail
		if(!closed) {
			shutdown(socketFile, SHUT_RDWR);
			::close(socketFile);
			closed = true;
		}
	}

	public: bool isClosed() { return closed; }
};

#endif