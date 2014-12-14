#include "NamingMessenger.h"

#include "Player.h"
#include "Messenger.h"
#include "LobbyMessenger.h"

using namespace rapidjson;
using namespace std;

NamingMessenger::NamingMessenger(Player* player) : Messenger(player) { }

NamingMessenger::~NamingMessenger() { }

void NamingMessenger::tellWorldInternal(Document& doc) {
	if(doc.HasMember("name")) { //If message is a name

		player->setName( doc.Get("name").GetString() ); //Set the name on the player

		stringstream s;
		s << "{\"playerId\":" << playerId() << "}";
		player->tell(s.str());

		Messenger* next = new LobbyMessenger(player);
		player->setMessenger(next); //Reassign the messenger
	}
}

void NamingMessenger::playerDied() {	} //At this point the player will clean up after itself