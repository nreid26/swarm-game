
#include "bin/$.h"
#include "bin/WebSocketServer.h"
#include <iostream>

using namespace std;

int main() {
	$WebSocketServer server = new WebSocketServer(6113);
	$WebSocket socket = server->accept();

	while(true) {
		cout << socket->read()->c_str() << endl;
	}
}
