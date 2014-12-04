#ifndef _NAMING_MESSENGER
#define _NAMING_MESSENGER

#include "$.h"
#include "Messenger.h"
#include "LobbyMessenger.h"

class NamingMessenger : public Messenger {
	//Data

	//Constructor
	public: NamingMessenger($Player player) : Messenger(player) {
		request();
	}

	//Methods
	protected: virtual void tellWorldInternal($<Document> doc) {
		if(doc->HasMember("name") && doc->get("name").IsString()) { //If message is a name
			player->setName(new String(doc->get("name").GetString())); //Set the name on the player

			$Messenger next = new LobbyMessenger(player);
			player->setMessenger(next); //Reassign the messenger
		}
		else { request(); }
	}

	private: void request() { player->tell("{name:true}"); }

};

#endif