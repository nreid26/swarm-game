#include "Lobby.h"

#include <string>
#include <map>
#include <vector>

#include "Semaphore.h"
#include "LobbyMessenger.h"
#include "Game.h"

#define min(a, b) (a < b) ? a : b;
#define max(a, b) (a > b) ? a : b;

using namespace std;

const string Lobby::JOINING("joining");
const string Lobby::LEAVING("leaving");
const string Lobby::CHALLENGED("challenged");

Lobby& Lobby::getInstance() {
	static Lobby instance; //Implicitly threadsafe singleton construction
	return instance;
}

Lobby::Lobby() : guard(1) { }

Lobby::~Lobby() {}

void Lobby::_registerMessenger(LobbyMessenger* messenger) {
	int search = searchMessengers(messenger->playerId()); //Search for this messenger in the vector

	if(search >= 0) { return; } //If the element is in there stop
	for(int i = 0; i < messengers.size(); i++) {
		LobbyMessenger* other = messengers[i];

		other->describeOtherPlayer(messenger->playerId(), messenger->playerName(), JOINING); //Tell existing messengers that this player is joining
		messenger->describeOtherPlayer(other->playerId(), other->playerName(), JOINING); //Tell the new player about the existing players
	}

	messengers.push_back(messenger); //Insert the new messenger into sorted position
	challenges.insert( pair<int, vector<int> >(search, vector<int>()) ); //Prepare a list to store challenges
}
void Lobby::registerMessenger(LobbyMessenger* messenger) { guard.wait(); _registerMessenger(messenger); guard.signal(); }

void Lobby::_unregisterMessenger(LobbyMessenger* messenger) {
	int search = searchMessengers( messenger->playerId() ); //Search for this messenger in the vector
	if(search < 0) { return; } //If the element is not in there stop

	cout << "Player " << messenger->playerId() << " is ungegistering" << endl;

	messengers.erase(messengers.begin() + search); //Remvove messenger at search position
	challenges.erase( messenger->playerId() ); //Delete the list of challenges made by this player

	for(int i = 0; i < messengers.size(); i++) {
		LobbyMessenger* other = messengers[i];

		other->describeOtherPlayer(messenger->playerId(), messenger->playerName(), LEAVING); //Tell existing messengers that this player is leaving

		for(auto itr = challenges[other->playerId()].begin(); itr != challenges[other->playerId()].end();) { //Iterate over each challenge vector
			if(*itr == messenger->playerId()) { challenges[other->playerId()].erase(itr); }
			else { itr++; }
		}
	}
}
void Lobby::unregisterMessenger(LobbyMessenger* messenger) { guard.wait(); _unregisterMessenger(messenger); guard.signal(); }

void Lobby::_issueChallenge(int fromPlayer, int toPlayer) {
	int searchTo = searchMessengers(toPlayer);
	int searchFrom = searchMessengers(fromPlayer);
	if(searchFrom >= 0 && searchTo >= 0 && searchTo != searchFrom) { //If both players are in lobby and are not the same player

		//Check to ensure this challenge is unique
		for(auto itr = challenges[fromPlayer].begin(); itr != challenges[fromPlayer].end(); itr++) { //Iterate over each challenge vector
			if(*itr == toPlayer) { return; } //No double challenging
		}

		challenges[fromPlayer].push_back(toPlayer); //If so, add it and realy message
		messengers[searchTo]->relayChallenge(fromPlayer);
		messengers[fromPlayer]->describeOtherPlayer(toPlayer, messengers[searchTo]->playerName(), CHALLENGED); //Confirm that player was challenged
	}
}
void Lobby::issueChallenge(int fromPlayer, int toPlayer) { guard.wait(); _issueChallenge(fromPlayer, toPlayer); guard.signal(); }


void Lobby::_acceptChallenge(int toPlayer, int fromPlayer) {
	int searchTo = searchMessengers(toPlayer); //Location of challenged player
	int searchFrom = searchMessengers(fromPlayer);  //Location of challenging player
	if(searchFrom >= 0 && searchTo >= 0 && searchTo != searchFrom) { //If both players are in lobby and are not the same player

		//Check to ensure this challenge exists
		for(auto itr = challenges[fromPlayer].begin(); itr != challenges[fromPlayer].end(); itr++) { //Iterate over each challenge vector
			if(*itr == toPlayer) { 
				LobbyMessenger* to = messengers[searchTo], *from = messengers[searchFrom];

				_unregisterMessenger(to);
				_unregisterMessenger(from);

				Game* g = new Game();
				g->addMessenger(to->beginGame(g));
				g->addMessenger(to->beginGame(g));
			}
		}
	}
}
void Lobby::acceptChallenge(int toPlayer, int fromPlayer) { guard.wait(); _acceptChallenge(toPlayer, fromPlayer); guard.signal(); }

int Lobby::searchMessengers(int key) {
	for(int i = 0; i < messengers.size(); i++) {
		if(messengers[i]->playerId() == key) {
			return i;
		}
	}
	return -1;
}
