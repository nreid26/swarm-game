#ifndef _MESSENGER
#define _MESSENGER

#include <sstream>
#include <string>

#include "rapidjson/include.h"

class Player;
class Game;

using namespace rapidjson;
using namespace std;

class Messenger {
	protected: Player* player;

	public: Messenger(Player* player);
	public: virtual ~Messenger();

	public: void tellWorld(string message);
	protected: virtual void tellWorldInternal(Document& doc) = 0;
	public: void tellPlayer(string message);
	public: int playerId();
	public: const string& playerName();
	public: virtual void playerDied() = 0;
};

#endif