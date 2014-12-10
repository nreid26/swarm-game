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
	private: $Lobby lobby;
	private: int gameBegun;

	//Constructor
	public: LobbyMessenger($Player player) : Messenger(player), gameBegun(false), lobby(Lobby::getInstance()) /*/assign lobby/*/ { }

	//Methods
	public: void describeOtherPlayer(int playerId, $String playerName, $String state) {
		if(gameBegun) { return; } //Do nothing if the game has started

		stringstream s;

		s << "{" <<
				"\"player\":{" <<
					"\"id\":" << playerId << ", " <<
					"\"name\":" << "\"" << playerName->c_str() << "\", " <<
					"\"state\":" << "\"" << state->c_str() << "\"" << 
				"}" <<
			"}";

		player->tell(new String(s.str()));
	}

	public: void relayChallenge(int challenger) {
		if(gameBegun) { return; } //Do nothing if the game has started

		stringstream s;

		s << "{" <<
				"\"challenge\":" << challenger <<
			"}";

		player->tell(new String(s.str()));
	}

	protected: virtual void tellWorldInternal($<Document> doc) {
		if(doc->HasMember("challenge")) {
			lobby->issueChallenge(player->getId(), doc->Get("challenge").GetInt());
		}

		else if(doc->HasMember("accept")) {
			lobby->acceptChallenge(doc->Get("challenge").GetInt());
		}
	}

	public: $GameMessenger beginGame($Game game) {
		gameBegun = true;

		$GameMessenger gm = new GameMessenger(player, game);
		player->setMessenger(gm);
		return gm;
	}

	public: virtual void playerDied() {
		lobby->unregisterMessenger(player->getId());
	}
};

typedef $<LobbyMessenger> $LobbyMessenger;

#endif