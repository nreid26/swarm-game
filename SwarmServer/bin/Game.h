#ifndef _GAME
#define _GAME

#include "$.h";
#include "GameMessenger.h"
#include "Exception.h"

class Game {
	//Data
	$GameMessenger messenger1, messenger2;

	//Constructor
	public: Game() {}

	//Methods
	public: void setMessenger($GameMessenger gm) {
		if(messenger1 == NULL) { messenger1 = gm; }
		else if(messenger2 == NULL) { messenger2 = gm; }
		else { throw new Exception("Game Cannot Have More Than Two Players"); }
	}
};

typedef $<Game> $Game;

#endif