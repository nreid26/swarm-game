#ifndef _LOBBY_MESSENGER
#define _LOBBY_MESSENGER

#include "Messenger.h"
#include "Lobby.h"
#include "Game.h"
#include "GameMessenger.h"

using namespace std;
using namespace rapidjson;

class LobbyMessenger : public Messenger {
	//Data
	private: int gameBegun;

	//Constructor
	public: LobbyMessenger(Player* player) : Messenger(player), gameBegun(false) {
		Lobby::getInstance().registerMessenger(this);
	}

	public: virtual ~LobbyMessenger() { }

	//Methods
	public: void describeOtherPlayer(int playerId, string& playerName, string& state) {
		if(gameBegun) { return; } //Do nothing if the game has started

		stringstream s;

		s << "{" <<
				"\"player\":{" <<
					"\"id\":" << playerId << ", " <<
					"\"name\":" << "\"" << playerName << "\", " <<
					"\"state\":" << "\"" << state << "\"" << 
				"}" <<
			"}";

		player->tell(s.str());
	}

	public: void relayChallenge(int challenger) {
		if(gameBegun) { return; } //Do nothing if the game has started

		stringstream s;

		s << "{" <<
				"\"challenge\":" << challenger <<
			"}";

		player->tell(s.str());
	}

	protected: virtual void tellWorldInternal(Document& doc) {
		if(doc.HasMember("challenge")) {
			Lobby::getInstance().issueChallenge(playerId(), doc.Get("challenge").GetInt());
		}

		else if(doc.HasMember("accept")) {
			Lobby::getInstance().acceptChallenge(playerId(), doc.Get("challenge").GetInt());
		}
	}

	public: GameMessenger* beginGame(Game* game) {
		gameBegun = true;

		GameMessenger* gm = new GameMessenger(player, game);
		player->setMessenger(gm);
		return gm;
	}

	public: virtual void playerDied() {
		Lobby::getInstance().unregisterMessenger(player->getId());
	}
};

#endif