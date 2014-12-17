#ifndef _LOBBY
#define _LOBBY

#include <string>
#include <map>
#include <vector>
#include "Semaphore.h"

class LobbyMessenger;
class Game;

using namespace std;

class Lobby {
	//Statics
	public: static Lobby& getInstance();

	private: static const string LEAVING;
	private: static const string JOINING;

	private: vector<LobbyMessenger*> messengers; //List of messengers sorted by player id ascending
	private: map<int, vector<int> > challenges; //Map of (issued by) => (issued to)
	private: Semaphore guard;
	private: vector<Game*> games;

	private: Lobby();
	public: virtual ~Lobby();

	private: void _registerMessenger(LobbyMessenger* messenger);
	public: void registerMessenger(LobbyMessenger* messenger);

	private: void _unregisterMessenger(LobbyMessenger* messenger);
	public: void unregisterMessenger(LobbyMessenger* messenger);

	private: void _issueChallenge(int fromPlayer, int toPlayer);
	public: void issueChallenge(int fromPlayer, int toPlayer);

	private: void _acceptChallenge(int toPlayer, int fromPlayer);
	public: void acceptChallenge(int toPlayer, int fromPlayer);

	private: int searchMessengers(int key);
};

#endif