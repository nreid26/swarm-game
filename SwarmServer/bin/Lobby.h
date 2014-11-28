#ifndef _LOBBY
#define _LOBBY

#include "$.h"
#include "Exception.h"
#include "LobbyMessenger.h"
#include <unordered_set>

using namespace std;

class Lobby {
	//Data
	private: $<unordered_set<$<LobbyMessenger>>> messengers;

	//Constructor
	public: Lobby() {}

	public: virtual ~Lobby() {}

	//Methods
	public: void registerMessenger($<LobbyMessenger> messenger) { 
		auto pair = messengers->insert(messenger); //Register messenger

		if(pair.second) { //If the messenger is unique, notify the other players in the lobby
			for(auto itr = messengers->begin(); itr != messengers->end(); itr++) {
				$<LobbyMessenger> itrMessenger = *itr;

				itrMessenger->describeOtherPlayer(messenger->playerId(), messenger->playerName(), "joining"); //Tell existing messengers that this player is joining
				messenger->describeOtherPlayer(itrMessenger->playerId(), itrMessenger->playerName(), "joining"); //Tell the new player about the existing players
			}
		}
	}

	public: void unregister($<LobbyMessenger> messenger) { 
		messengers->erase(messenger); 

		for(auto itr = messengers->begin(); itr != messengers->end(); itr++) {
			itr->operator->describeOtherPlayer(messenger->getId(), messenger->getName(), "leaving");
		}

	}

};

#endif