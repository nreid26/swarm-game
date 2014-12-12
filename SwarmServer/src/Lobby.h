#ifndef _LOBBY
#define _LOBBY

#include <string>
#include <map>
#include <vector>
#include "Semaphore.h"

class LobbyMessenger;

using namespace std;

class Lobby {
	//Statics
	public: static Lobby& getInstance();

	private: static const string LEAVING;
	private: static const string JOINING;
	private: static const string CHALLENGED;

	private: vector<LobbyMessenger*> messengers; //List of messengers sorted by player id ascending
	private: map<int, vector<int> > challenges; //Map of (issued by) => (issued to)
	private: Semaphore guard;

	private: Lobby();
	public: virtual ~Lobby();

	private: void _registerMessenger(LobbyMessenger* messenger);
	public: void registerMessenger(LobbyMessenger* messenger);

	private: void _unregisterMessenger(int playerId);
	public: void unregisterMessenger(int playerId);

	private: void _issueChallenge(int fromPlayer, int toPlayer);
	public: void issueChallenge(int fromPlayer, int toPlayer);

	private: void _acceptChallenge(int toPlayer, int fromPlayer);
	public: void acceptChallenge(int toPlayer, int fromPlayer);

	private: int searchMessengers(int key);
	private: int searchMessengers(int key, int start, int end);
};

#endif