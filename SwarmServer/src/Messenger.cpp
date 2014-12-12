#include "Messenger.h"

#include "Player.h"

using namespace rapidjson;
using namespace std;

Messenger::Messenger(Player* player) : player(player) {}

Messenger::~Messenger() {}

void Messenger::tellWorld(string message) {
	Document doc;
	doc.Parse<0>(&(message[0]));

	if(!doc.HasParseError() && doc.IsObject()) {
		try { tellWorldInternal(doc); }
		catch(...) {}
	}
}

void Messenger::tellPlayer(string message) { player->tell(message); }

int Messenger::playerId() { return player->getId(); }

const string& Messenger::playerName() { return player->getName(); }

