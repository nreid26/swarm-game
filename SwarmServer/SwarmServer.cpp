
#include "src/$.h"
#include "src/WebSocketServer.h"
#include "src/Exception.h"

#include <iostream>

using namespace std;

int main() {
	try {
		$WebSocketServer server = new WebSocketServer(6113);
		$WebSocket socket = server->accept();

		cout << "Connection Says:" << socket->read()->c_str() << endl;
	}
	catch(Exception* e) {
		cerr << e->getMessage()->c_str() << endl;
	}
}
