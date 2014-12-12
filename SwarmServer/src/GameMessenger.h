#ifndef _GAME_MESSENGER
#define _GAME_MESSENGER

#include <time.h>

#include "Messenger.h"

using namespace std;
using namespace rapidjson;

class GameMessenger : public Messenger {
	public: static unsigned long unixMillis();

	private: Game* game;

	public: GameMessenger(Player* player, Game* game);
	public: virtual ~GameMessenger();

	protected: virtual void tellWorldInternal(Document& doc);
	public: void relayPlanet(int id, int x, int y, int z, int capacity, int growth, int playerId, int troops);
	public: void relayWinner(int winner);
	public: void relayUpdate(int dest, int playerId, int troops);
	public: void relayDeployment(int source, int destination, int playerId, int  troops, int arrivalTime);
	public: void relayTerminator(int radius, int minSep);
	public: virtual void playerDied();
};

#endif