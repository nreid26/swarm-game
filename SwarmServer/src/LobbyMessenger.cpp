#include "LobbyMessenger.h"

#include "Lobby.h"
#include "Game.h"
#include "GameMessenger.h"
#include "Player.h"

using namespace std;
using namespace rapidjson;

LobbyMessenger::LobbyMessenger(Player* player) : Messenger(player), gameBegun(false) {
	Lobby::getInstance().registerMessenger(this);
}

LobbyMessenger::~LobbyMessenger() { }

void LobbyMessenger::describeOtherPlayer(int playerId, const string& playerName, const string& state) {
	if(gameBegun) { return; } //Do nothing if the game has started

	stringstream s;

	s << "{" <<
			"\"player\":{" <<
				"\"id\":" << playerId << ", " <<
				"\"name\":" << "\"" << playerName << "\", " <<
				"\"state\":" << "\"" << state << "\"" << 
			"}" <<
		"}";

	cout << "described player as " << s.str() << endl;

	player->tell(s.str());
}

void LobbyMessenger::relayChallenge(int challenger) {
	if(gameBegun) { return; } //Do nothing if the game has started

	stringstream s;

	s << "{" <<
			"\"challenge\":" << challenger <<
		"}";

	player->tell(s.str());
}

void LobbyMessenger::tellWorldInternal(Document& doc) {
	if(doc.HasMember("challenge")) {
		Lobby::getInstance().issueChallenge(playerId(), doc.Get("challenge").GetInt());
	}

	else if(doc.HasMember("accept")) {
		Lobby::getInstance().acceptChallenge(playerId(), doc.Get("challenge").GetInt());
	}
}

GameMessenger* LobbyMessenger::beginGame(Game* game) {
	gameBegun = true;

	GameMessenger* gm = new GameMessenger(player, game);
	player->setMessenger(gm);
	return gm;
}

void LobbyMessenger::playerDied() {
	Lobby::getInstance().unregisterMessenger(this);
	cout << "lobby messenger player died" << endl;
}