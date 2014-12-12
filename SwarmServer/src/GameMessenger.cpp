#include "GameMessenger.h"

#include <time.h>

#include "Player.h"
#include "Lobby.h"
#include "Game.h"
#include "LobbyMessenger.h"

using namespace std;
using namespace rapidjson;

unsigned long GameMessenger::unixMillis() {
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return (unsigned long long)(tv.tv_sec) * 1000 + (unsigned long long)(tv.tv_usec) / 1000 / 1000;
}

GameMessenger::GameMessenger(Player* player, Game* game) : Messenger(player), game(game) {}

GameMessenger::~GameMessenger() { }

void GameMessenger::tellWorldInternal(Document& doc) {
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

void GameMessenger::relayPlanet(int id, int x,int y,int z, int capacity, int growth, int playerId, int troops){
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
	
void GameMessenger::relayWinner(int winner){
	stringstream s;

	s << "{" << 
			"\"winner\":" << winner <<
		"}";

	player->tell(s.str());

	LobbyMessenger* mes = new LobbyMessenger(player);
	player->setMessenger(mes);
}
	
void GameMessenger::relayUpdate(int dest, int playerId, int troops){
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
	
void GameMessenger::relayDeployment(int source, int destination, int playerId,int  troops, int arrivalTime){
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

void GameMessenger::relayTerminator(int radius, int minSep) {
	stringstream s;

	s << "{" <<
		"\"radius\":" << radius << ", " <<
		"\"minimumSeparation\":" << minSep <<
		"}";

	player->tell(s.str());
}

void GameMessenger::playerDied() {
	game->surrender(this, false);
}