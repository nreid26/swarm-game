SwarmServer: SwarmServer.o Messenger.o GameMessenger.o LobbyMessenger.o NamingMessenger.o ConnectionDelegate.o Player.o Game.o Lobby.o
	g++ SwarmServer.cpp Messenger.o GameMessenger.o LobbyMessenger.o NamingMessenger.o ConnectionDelegate.o Player.o Game.o Lobby.o -o SwarmServer -pthread -std=c++11 -l rt

GameMessenger.o: src/GameMessenger.cpp
	g++ src/GameMessenger.cpp -c -pthread -std=c++11

LobbyMessenger.o: src/LobbyMessenger.cpp
	g++ src/LobbyMessenger.cpp -c -pthread -std=c++11

NamingMessenger.o: src/NamingMessenger.cpp
	g++ src/NamingMessenger.cpp -c -pthread -std=c++11

Messenger.o: src/Messenger.cpp
	g++ src/Messenger.cpp -c -pthread -std=c++11

ConnectionDelegate.o: src/ConnectionDelegate.cpp
	g++ src/ConnectionDelegate.cpp -c -pthread -std=c++11

Game.o: src/Game.cpp
	g++ src/Game.cpp -c -pthread -std=c++11

Lobby.o: src/Lobby.cpp
	g++ src/Lobby.cpp -c -pthread -std=c++11

Player.o: src/Player.cpp
	g++ src/Player.cpp -c -pthread -std=c++11

clean:
	rm -rf *o SwarmServer