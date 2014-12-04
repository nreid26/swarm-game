#ifndef _LOBBY_MESSENGER
#define _LOBBY_MESSENGER

#include "Messenger.h"
#include "Lobby.h"
#include "Game.h"
#include "GameMessenger.h"

using namespace std;

class LobbyMessenger : public Messenger {
	//Data
	private: $Lobby lobby;
	private: int gameBegun;

	//Constructor
	public: LobbyMessenger($Player player) : Messenger(player), gameBegun(false), lobby(Lobby::getInstance()) /*/assign lobby/*/ { }

	//Methods
	public: void describeOtherPlayer(int playerId, $String playerName, $String state) {
		if(gameBegun) { return; } //Do nothing if the game has started

		//Serialize this data into json an give to player
	}

	public: void relayChallenge(int challenger) {
		if(gameBegun) { return; } //Do nothing if the game has started

		//Send json to player indicating a challenge was made by challenger
	}

	public: void beginGame($Game game) {
		gameBegun = true;
		$GameMessenger gm = new GameMessenger(player, game);

		game->setMessenger(gm);
		player->setMessenger(gm);
	}
};

typedef $<LobbyMessenger> $LobbyMessenger;

#endif