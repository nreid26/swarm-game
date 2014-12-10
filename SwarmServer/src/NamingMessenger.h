#ifndef _NAMING_MESSENGER
#define _NAMING_MESSENGER

#include "$.h"
#include "Messenger.h"
#include "LobbyMessenger.h"

using namespace rapidjson;
using namespace std;

class NamingMessenger : public Messenger {
	//Data

	//Constructor
	public: NamingMessenger($Player player) : Messenger(player) { }

	public: virtual ~NamingMessenger() { }

	//Methods
	protected: virtual void tellWorldInternal($<Document> doc) {
		if(doc->HasMember("name")) { //If message is a name

			player->setName(new String(doc->Get("name").GetString())); //Set the name on the player

			$Messenger next = new LobbyMessenger(player);
			player->setMessenger(next); //Reassign the messenger
		}
	}

	protected: virtual void playerDied() {	} //At this point the player will clean up after itself
};

#endif