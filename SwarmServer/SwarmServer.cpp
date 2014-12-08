
#include "src/$.h"
#include "src/WebSocketServer.h"
#include <iostream>

using namespace std;

int main() {
	//$WebSocketServer server = new WebSocketServer(6113);
	//$WebSocket socket = server->accept();

	$String key = "dGhlIHNhbXBsZSBub25jZQ==258EAFA5-E914-47DA-95CA-C5AB0DC85B11";

	$String hash = SHA1::calc(key);
	cout << hash->c_str() << endl;

	$String ret = SHA1::toBase64String(hash);
	cout << ret->c_str() << endl;

	//cout << "Connection Says:" << socket->read()->c_str() << endl;
	
	//socket->close();
}
