#ifndef _GAME
#define _GAME

#define min(a, b) ((a > b) ? b : a)

#include "$.h";
#include "GameMessenger.h"
#include "LobbyMessenger.h"
#include "Exception.h"
#include "Thread.h"
#include "Semaphore.h"
#include "ThreadDisposer.h"

#include <string>
#include <stdio.h>      /* printf, NULL */
#include <stdlib.h>     /* srand, rand */
#include <time.h>
#include <cmath>

#include <vector>

#include <unistd.h> //Sleep

class Game : public Thread<void> {
	//Classes
	private: class Planet {
		public: int capacity, growth, player, troops;
		public: int x, y, z;
		public: int id;
	};

	private: class Deployment : public Thread<void> {
		//Data
		public: int player, troops, wait, dest;
		public: $Game parent;
		private: $<Deployment> myself;

		//Constructor
		public: Deployment(int p, int t, int w, int d, $Game g) : player(p), troops(t), wait(w), dest(d), parent(g) { }

		//Methods
		protected: $<void> run() {
			usleep(delay * 1000); //Wait for delay ms

			parent->arriveDeployment(troops, dest, player);
			ThreadDisposer::getInstance()->add(myself);

			parent = NULL;
			myself = NULL;

			cancel(); //Only run once
			return NULL;
		}

		public: void start($<Deployment> myself) {
			myself->myself = myself;
			Thread::start();
		}
	};

	//Statics
	private: const static double PI;
	private: const static double TROOP_SPEED;
	private: const static int radius = 100;
	private: const static int minSeparation = 7;

	public: static $Game createGame($LobbyMessenger a, $LobbyMessenger b) {
		$Game temp = new Game();
		temp->myself = temp;

		temp->messenger1 = a->beginGame(temp);
		temp->messenger2 = b->beginGame(temp);
	}

	//Data
	private: $GameMessenger messenger1, messenger2;
	private: $Game myself; //Reference to this object;
	private: vector<Planet> planets;
	private: vector<vector<int>> timeMatrix; //Milliseconds between planets
	private: vector<$<Deployment>>; //So that that the game may delete the 

	private: int depCount = 0;
	private: int winner = -1;

	private: Semaphore guard; //Guard on planets and depCount

	//Constructor
	private: Game() : guard(0) { generate(); }

	//Methods
		//Building
	private: void generate() {
		//Generate each octant of the map
		for(int a = -1; a < 3; a += 2) {
			for(int b = -1; b < 3; b += 2) {
				for(int c = -1; c < 3; c += 2) {
					generateSector(a, b, c, 500);
				}
			}
		}

		for(int a = 0; a < planets.size(); a++) {
			for(int b = 0; b < planets.size(); b++) {
				timeMatrix[a][b] = (int)(distance(planets[a], planets[b]) / TROOP_SPEED * 1000);
			}
		}
	}

	private: void generateSector(double sp, double st, int mag, int sum) {
		Planet p;

		p.player = -1;
		p.troops = 50;

		while(sum > 0) {
			p.growth = 2 * ((int)(4 * randunit()) + 1);  //Set the growth of the planet from 2 * (1..5)
			p.capacity = 50 * p.growth; //Set capacity to 50 times growth rate

			sum -= p.capacity; //Reduce remaning capacity in this slice

			//Assign and check positions until a valid one is found
			while(true) {
				int phi = randAngle(sp);
				int theta = randAngle(st);

				p.z = sin(phi) * mag;
				p.y = cos(phi) * sin(theta) * mag;
				p.x = cos(phi) * cos(theta) * mag;

				int i;
				for(i = 0; i < planets.size() && distanceSquared(planets[i], p) >= minSeparation * minSeparation; i++) {} //Ensure this new planet is at least minSeparation from all others
				if(i == planets.size()) { break; }
			}

			//Add cells to the time matrix
			timeMatrix.emplace_back);
			for(int i = 0; i < timeMatrix.size(); i++) { timeMatrix.push_back(0); }

			p.id = planets.size();
			planets.push_back(p); //Copy the planet into the vector
		}
	}

	private: double randUnit() { //Random number 0..1
		static void srand(time(NULL));
		return (rand() % 1000 + 1) / 1000.0;
	}

	private: double randAngle(double sign) { return  PI / 2 * randUnit() * sign; } //Random angle 0..PI/2, signed

	private: double distanceSquared(Planet a, Planet b) { return pow(a.x - b.x, 2) + pow(a.y - b.y, 2) + pow(a.z - b.z, 2); } //Distance between two planets

		//Play
	public: void start() { //Gaurd is 0 until this call so the game state is unmodifiable externally
		Thread::start();
		guard.signal(); //Let the games begin
	}

	protected: $<void> run() { //Conduct troop growth every 100ms
		guard.wait();

			for(int i = 0; i < planets.size(); i++) {
				if(planets[i].player >= 0) { planets[i].troops += planets[i].growth;} //Only increment players' planets
				planets[i].troops = min(planets[i].troops, planets[i].capacity) //Ensure troops never exceeds capacity
			}

		guard.signal();

		usleep(100 * 1000);

		return NULL;
	}

	public: void sendDeployment(int playerId, int sourceId, int destId) {
		guard.wait();
			if(planets[sourceId].player != playerId) { return; } //If the player does not own the source planet do nothing

			//Take half the troops from the source planet
			int troops = planets[sourceId].troops / 2;
			planets[sourceId].troops -= troops;

			//Create a deployment thread
			depCount++;
			$<Deployment> dep = new Deployment(playerId, troops, timeMatrix[sourceId][destId], destId, myself);
			dep->start(dep);
		guard.signal();

		//Tell the players about this deployment
		messenger1->relayDeployment(sourceId, destId, playerId, troops, timeMatrix[sourceId][destId]);
		messenger2->relayDeployment(sourceId, destId, playerId, troops, timeMatrix[sourceId][destId]);
	}

	public: void arriveDeployment(int troops, int dest, int player) {
		guard.wait();

			depCount--; //Discount this dep

			if(planets[dest].player == player) { planets[dest].troops += troops; } //If the player owns the planet
			else if(planets[dest].troops >= troops) { planets[dest].troops -= troops;} //If the planet has more enemies
			else { //If the planet has fewer enemies
				planets[dest].troops = troops - planets[dest].troops;
				planets[dest].player = player;
			}
			planets[dest].troops = min(planets[dest].troops, planets[dest].capacity); //Don't exceed planet capacity

			testWinner(); //Test for and declare winner
			if(winner >= 0) {
				messenger1->relayWinner(hasWon);
				messenger2->relayWinner(hasWon);

				ThreadDisposer::getInstance()->add(myself);
				cancel();
				myself = NULL; //Eliminate this soon to be lost reference
			}
			else { //Update the state of that planet on the client side
				messenger1->relayUpdate(dest, planets[dest].player, planets[dest].troops);
				messenger2->relayUpdate(dest, planets[dest].player, planets[dest].troops);

				guard.signal();
			}
	}

	private: void testWinner() { //Is there a winner
		//Count the planets owned by each player
		int p1(0), p2(0);
		for(int i = 0; i < planets.size(); i++) {
			if(planets[i].player == messenger1->playerId()) { p1++; }
			else if(planets[i].player == messenger2->playerId()) { p2++; }
		}

		//If a player owns none, return the id of the other
		if(p1 == 0) { winner = messenger2->playerId(); }
		else if(p2 == 0) { winner = messenger2->playerId(); }
	}	
};

const double Game::PI = 3.141592654;
const double Game::TROOP_SPEED = 2 * radius / 25;  //units / seconds

typedef $<Game> $Game;

#endif