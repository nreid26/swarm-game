#ifndef _GAME_MESSENGER
#define _GAME_MESSENGER

#include "Player.h"
#include "Lobby.h"
#include "Game.h"
#include "Messenger.h"

#include <time.h>

class GameMessenger : public Messenger {
	//Statics	
	public: static unsigned long unixMillis() {
		struct timeval tv;
		gettimeofday(&tv, NULL);
		return (unsigned long long)(tv.tv_sec) * 1000 + (unsigned long long)(tv.tv_usec) / 1000 / 1000;
	}
	//Data
	private: Game* game;

	//Constructor
	public: GameMessenger(Player* player, Game* game) : Messenger(player), game(game) {}

	public: virtual ~GameMessenger() { }

	//Methods
	protected: virtual void tellWorldInternal(Document& doc) {
		if(doc.HasMember("deployment")) {
			const Value& a = doc["deployment"];

			int source = a["source"].GetInt();
			int dest = a["destination"].GetInt();

			game->sendDeployment(player->getId(), source, dest);
		}

		else if(doc.HasMember("lobby")) {
			game->surrender(this, true);
		}
	}

	public: void relayPlanet(int id, int x,int y,int z, int capacity, int growth, int playerId, int troops){
		stringstream s;

		s << "{" << 
				"\"planet\":{" << 
					"\"id\":" << id << ", " << 
					"\"capacity\":" << capacity << ", " <<
					"\"growth\":" << growth << ", " <<
					"\"player\":" << playerId << ", " <<
					"\"troops\":" << troops << ", " <<
					"\"position\":{" << 
						"\"x\":" << x << ", " << 
						"\"y\":" << y << ", " << 
						"\"z\":" << z << "" << 
					"}" <<
				"}" <<
			"}";

		player->tell(s.str());
	}
	
	public: void relayWinner(int winner){
		stringstream s;

		s << "{" << 
				"\"winner\":" << winner <<
			"}";

		player->tell(s.str());

		LobbyMessenger* mes = new LobbyMessenger(player);
		player->setMessenger(mes);
	}
	
	public: void relayUpdate(int dest, int playerId, int troops){
		stringstream s;

		s << "{" << 
				"\"planet\":{" << 
					"\"id\":" << dest << ", " << 
					"\"player\":" << playerId << ", " <<
					"\"time\":" << unixMillis() << ", " <<
					"\"troops\":" << troops << "" <<
				"}" <<
			"}";

		player->tell(s.str());
	}
	
	public: void relayDeployment(int source, int destination, int playerId,int  troops, int arrivalTime){
		stringstream s;

		s << "{" << 
				"\"deployment\":{" << 
					"\"source\":" << source << ", " << 
					"\"destination\":" << destination << ", " <<
					"\"player\":" << playerId << ", " <<
					"\"troops\":" << troops << ", " <<
					"\"arrival\":" << unixMillis() + arrivalTime <<
				"}" <<
			"}";

		player->tell(s.str());
	}

	public: void relayTerminator(int radius, int minSep) {
		stringstream s;

		s << "{" <<
			"\"radius\":" << radius << ", " <<
			"\"minimumSeparation\":" << minSep <<
			"}";

		player->tell(s.str());
	}

	public: virtual void playerDied() {
		game->surrender(this, false);
	}
};

#endif