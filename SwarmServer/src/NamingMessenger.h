#ifndef _NAMING_MESSENGER
#define _NAMING_MESSENGER

#include "Messenger.h"

using namespace rapidjson;
using namespace std;

class NamingMessenger : public Messenger {
	public: NamingMessenger(Player* player);
	public: virtual ~NamingMessenger();

	protected: virtual void tellWorldInternal(Document& doc);
	protected: virtual void playerDied();
};

#endif