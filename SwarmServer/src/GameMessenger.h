#ifndef _GAME_MESSENGER
#define _GAME_MESSENGER

#include "$.h"
#include "Player.h"
#include "Game.h"

class GameMessenger {
	//Data
	private: $Player player;
	private: $Game game;

	//Constructor
	public: GameMessenger($Player player, $Game game) : player(player), game(game) {}

	//Methods
	protected: virtual void tellWorldInternal($<Document> doc) {

	}
};

typedef $<GameMessenger> $GameMessenger;

#endif