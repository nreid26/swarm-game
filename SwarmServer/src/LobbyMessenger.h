#ifndef _LOBBY_MESSENGER
#define _LOBBY_MESSENGER

#include "Messenger.h"

class GameMessenger;

using namespace std;
using namespace rapidjson;

class LobbyMessenger : public Messenger {
	private: int gameBegun;

	public: LobbyMessenger(Player* player);
	public: virtual ~LobbyMessenger();

	public: void describeOtherPlayer(int playerId, const string& playerName, const string& state);
	public: void relayChallenge(int challenger);
	protected: virtual void tellWorldInternal(Document& doc);
	public: GameMessenger* beginGame(Game* game);
	public: virtual void playerDied();
};

#endif