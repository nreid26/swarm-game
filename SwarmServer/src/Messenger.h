#ifndef _MESSENGER
#define _MESSENGER

#include "rapidjson/include.h"

#include <sstream>
#include "$.h"
#include "Player.h"

using namespace rapidjson;
using namespace std;

class Messenger {
	//Data
	protected: $Player player;

	//Constructor
	public: Messenger($Player player) : player(player) {}

	//Methods
	public: void tellWorld($String message) {
		$<Document> doc = new Document();
		doc->Parse<0>(message->c_str);

		if(!doc->HasParseError() && doc->IsObject()) {
			try {
				tellWorldInternal(doc);
			}
			catch(...) {}
		}
	}

	public: virtual ~Messenger() {}

	protected: virtual void tellWorldInternal($<Document> doc) = 0;

	public: void tellPlayer($String message) { player->tell(message); }

	public: int playerId() { return player->getId(); }

	public: $String playerName() { return player->getName(); }

	public: virtual void playerDied() = 0;
};

typedef $<Messenger> $Messenger;
#endif