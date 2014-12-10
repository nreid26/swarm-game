#ifndef _GAME_MESSENGER
#define _GAME_MESSENGER

#include "$.h"
#include "Player.h"
#include "Game.h"
#include "Messenger.h"

class GameMessenger : public Messenger{
	//Data
	private: $Player player;
	private: $Game game;

	//Constructor
	public: GameMessenger($Player player, $Game game) : player(player), game(game) {}

	//Methods
	protected: virtual void tellWorldInternal($<Document> doc) {

	}

	public: beginGame($Game newGame){
		$Game game = newGame;
	}
	public: relayPlanet(int id, int x,int y,int z, int capacity, int growth, int player, int troops){
		stringstream s;

		s << "{" << 
				"\"planet\":{" << 
					"\"id\":" << id << ", " << 
					"\"capacity\":" << capacity << ", " <<
					"\"growth\":" << growth << ", " <<
					"\"player\":" << player << ", " <<
					"\"troops\":" << troops << ", " <<
					"\"position\":{" << 
						"\"x\":" << x << ", " << 
						"\"y\":" << y << ", " << 
						"\"z\":" << z << "" << 
					"}" <<
				"}" <<
			"}";

		player->tell(new String(s.str()));
	}
	public: relayWinner(int winner){
		stringstream s;

		s << "{" << 
				"\"winner\":" << winner <<
			"}";

		player->tell(new String(s.str()));
	}
	public: relayUpdate( int id, int player, int troops){
		stringstream s;

		s << "{" << 
				"\"planet\":{" << 
					"\"id\":" << dest << ", " << 
					"\"player\":" << player << ", " <<
					"\"troops\":" << troops << "" <<
				"}" <<
			"}";

		player->tell(new String(s.str()));
	}
	public: relayDeployment(int source, int destination, int player,int  troops, int arrivalTime){
		stringstream s;

		s << "{" << 
				"\"deployment\":{" << 
					"\"source\":" << source << ", " << 
					"\"destination\":" << destination << ", " <<
					"\"player\":" << player << ", " <<
					"\"troops\":" << troops << ", " <<
					"\"arrival\":" << arrivalTime << "" <<
				"}" <<
			"}";

		player->tell(new String(s.str()));
	}
};

typedef $<GameMessenger> $GameMessenger;

#endif