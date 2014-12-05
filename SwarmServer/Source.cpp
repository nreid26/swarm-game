
#include "src/$.h"
#include "src/WebSocketServer.h"
#include <iostream>

using namespace std;

int main() {
	$WebSocketServer server = new WebSocketServer(6113);
	$WebSocket socket = server->accept();

	cout << "Connection Says:" << socket->read()->c_str() << endl;
	
	socket->close();
}
