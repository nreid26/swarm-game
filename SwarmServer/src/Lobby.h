#ifndef _LOBBY
#define _LOBBY

#define min(a, b) (a < b) ? a : b;
#define max(a, b) (a > b) ? a : b;

#include "$.h"
#include "Exception.h"
#include "LobbyMessenger.h"
#include <map>
#include <vector>

using namespace std;

template <class T> using $Vector = $<vector<T>>;
template <class T, class U> using $Map = $<map<T, U>>;

const char LEAVING[8] = "leaving";
const char JOINING[8] = "joining";
const char CHALLENGED[11] = "challenged";

class Lobby {
	//Statics
	public: static $<Lobby> getInstance() {
		static $<Lobby> instance = new Lobby(); //Implicitly threadsafe singleton construction
		return instance;
	}

	//Data
	private: $Vector<$LobbyMessenger> messengers; //List of messengers sorted by player id ascending
	private: $Map<int, $Vector<int>> challenges; //Map of (issued by) => (issued to)

	//Constructor
	private: Lobby() : messengers(new vector<$LobbyMessenger>()), challenges(new map<int, $Vector<int>>) {}

	public: virtual ~Lobby() {}

	//Methods
	public: void registerMessenger($LobbyMessenger messenger) { 
		int search = searchMessengers(messenger->playerId()); //Search for this messenger in the vector

		if(search >= 0) { return; } //If the element is in there stop
		search = -(search + 1);

		for(int i = 0; i < messengers->size(); i++) {
			$LobbyMessenger other = messengers->at(i);

			other->describeOtherPlayer(messenger->playerId(), messenger->playerName(), JOINING); //Tell existing messengers that this player is joining
			messenger->describeOtherPlayer(other->playerId(), other->playerName(), JOINING); //Tell the new player about the existing players
		}

		messengers->insert(messengers->begin() + search, messenger); //Insert the new messenger into sorted position
		challenges->insert(challenges->begin(), pair<int, $Vector<int>>(messenger->playerId(), new vector<int>)); //Prepare a list to store challenges
	}

	public: void unregisterMessenger($LobbyMessenger messenger) { 
		int search = searchMessengers(messenger->playerId()); //Search for this messenger in the vector

		if(search < 0) { return; } //If the element is not in there stop

		messengers->erase(messengers->begin() + search); //Remvove messenger at search position
		challenges->erase(messenger->playerId()); //Delete the list of challenges made by this player

		for(int i = 0; i < messengers->size(); i++) {
			$LobbyMessenger other = messengers->at(i);

			other->describeOtherPlayer(messenger->playerId(), messenger->playerName(), LEAVING); //Tell existing messengers that this player is leaving

			$Vector<int> ch = challenges->at(other->playerId());
			for(auto itr = ch->begin(); itr != ch->end(); ) { //Iterate over each challenge vector
				if(*itr == messenger->playerId()) { ch->erase(itr); }
				else { itr++; }
			}
		}
	}

	public: void issueChallenge(int fromPlayer, int toPlayer) {
		int searchTo = searchMessengers(toPlayer);
		int searchFrom = searchMessengers(fromPlayer);
		if(searchFrom >= 0 && searchTo >= 0 && searchTo != searchFrom) { //If both players are in lobby and are not the same player

			//Check to ensure this challenge is unique
			$Vector<int> ch = challenges->at(fromPlayer);
			for(auto itr = ch->begin(); itr != ch->end(); itr++) { //Iterate over each challenge vector
				if(*itr == toPlayer) { return; } //No double challenging
			}

			ch->push_back(toPlayer); //If so, add it and realy message
			messengers->at(searchTo)->relayChallenge(fromPlayer);
		}
	}

	public: void acceptChallenge(int toPlayer, int fromPlayer) {
		int searchTo = searchMessengers(toPlayer); //Location of challenged player
		int searchFrom = searchMessengers(fromPlayer);  //Location of challenging player
		if(searchFrom >= 0 && searchTo >= 0 && searchTo != searchFrom) { //If both players are in lobby and are not the same player

			//Check to ensure this challenge exists
			$Vector<int> ch = challenges->at(fromPlayer);
			for(auto itr = ch->begin(); itr != ch->end(); itr++) { //Iterate over each challenge vector
				if(*itr == toPlayer) { 
					$LobbyMessenger to = messengers->at(searchTo), from = messengers->at(searchFrom);
					$Game game = new Game();

					to->beginGame(game);
					from->beginGame(game);

					unregisterMessenger(to);
					unregisterMessenger(from);
				}
			}
		}
	}


	private: int searchMessengers(int key) { return searchMessengers(key, 0, messengers->size()); } //Wrapper

	private: int searchMessengers(int key, int start, int end) { //Recursive binary search of messenger list		
		int mid = (start + end) / 2;
		int at = messengers->at(mid)->playerId();

		if(key == at) { return mid; }
		else if(start == end) { return -(1 + start); }
		else if(key < at) { return searchMessengers(key, start, mid); }
		else { return searchMessengers(key, mid, end); }
	}
};

typedef $<Lobby> $Lobby;

#endif