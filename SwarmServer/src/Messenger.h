#ifndef _MESSENGER
#define _MESSENGER

#include "rapidjson/include.h"

#include <sstream>
#include "Player.h"

using namespace rapidjson;
using namespace std;

class Messenger {
	//Data
	protected: Player* player;

	//Constructor
	public: Messenger(Player* player) : player(player) {}

	public: virtual ~Messenger() {}

	//Methods
	public: void tellWorld(string& message) {
		Document doc;
		doc.Parse<0>(&(message[0]));

		if(!doc.HasParseError() && doc.IsObject()) {
			try { tellWorldInternal(doc); }
			catch(...) {}
		}
	}

	protected: virtual void tellWorldInternal(Document& doc) = 0;

	public: void tellPlayer(string& message) { player->tell(message); }

	public: int playerId() { return player->getId(); }

	public: string playerName() { return player->getName(); }

	public: virtual void playerDied() = 0;
};

#endif