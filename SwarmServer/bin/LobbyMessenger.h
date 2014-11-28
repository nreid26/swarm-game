#ifndef _LOBBY_MESSENGER
#define _LOBBY_MESSENGER

#include "Messenger.h"

using namespace std;

class LobbyMessenger : public Messenger {
	//Data
	private: $<Lobby> lobby;

	//Constructor
	public: LobbyMessenger($<Player> player) : Messenger(player) {}

	//Methods
	public: describeOtherPlayer(int playerId, $<String> playerName, $<String> state) {
		//Serialize this data into json an give to player
	}
};

#endif