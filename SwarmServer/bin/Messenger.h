#ifndef _MESSENGER
#define _MESSENGER

#include <String>
#include <vector>
#include <cstdio>
#include "rapidjson/document.h"		// rapidjson's DOM-style API
#include "rapidjson/prettywriter.h"	// for Stringify JSON
#include "rapidjson/filestream.h"	// wrapper of C stream for prettywriter as output

#include "$.h"
#include "Player.h"

using namespace rapidjson;
using namespace std;

class Messenger {
	//Data
	private: $<Player> player;

	//Constructor
	public: Messenger($<Player> player) : player(player) {}

	//Methods
	public: void tellWorld($<String> message) {
		$<Document> doc = new Document();
		doc->Parse<0>(message->c_str);
		tellWorldInternal(doc);
	}

	public: virtual ~Messenger() {}

	protected: virtual void tellWorldInternal($<Document> doc) {}

	public: void tellPlayer($<String> message) { player->tell(message); }

	public: int playerId() { return player->getId(); }

	public: $<String> playerName() { return player->getName(); }
};


#endif