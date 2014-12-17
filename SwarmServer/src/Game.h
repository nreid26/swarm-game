#ifndef _GAME
#define _GAME

#include <string>
#include <stdio.h>      /* printf, NULL */
#include <stdlib.h>     /* srand, rand */
#include <time.h>
#include <cmath>
#include <vector>

#include "Exception.h"
#include "Thread.h"
#include "Semaphore.h"
#include "ThreadDisposer.h"

class GameMessenger;
class LobbyMessenger;

class Game : public Thread<int> {
	private: class Planet {
		public: int capacity, growth, player, troops;
		public: int x, y, z;
		public: int id;
	};
	private: class Deployment : public Thread<int> {
		//Data
		public: int player, troops, wait, dest;
		public: Game* game;

		//Constructor
		public: Deployment(int p, int t, int w, int d, Game* g) : player(p), troops(t), wait(w), dest(d), game(g) { 
			start();
		}

		public: virtual ~Deployment() {	}

		//Methods
		protected: int* run() {
			usleep(wait * 1000); //Wait for delay ms

			game->arriveDeployment(troops, dest, player);

			cancel(); //Only run once
			return NULL;
		}
	};

	private: const static double PI;
	private: const static double TROOP_SPEED;
	private: const static double CENT_TIME;
	private: const static int systemRadius;
	private: const static int minSeparation;

	private: GameMessenger* messenger1, *messenger2;
	private: vector<Planet> planets;
	private: vector<vector<int> > timeMatrix; //Milliseconds between planets
	private: vector<Deployment*> deployments;

	private: int depCount;
	private: int winner;

	private: Semaphore guard; //Guard on planets and depCount

	public: Game();
	public: virtual ~Game();

	public: void addMessenger(GameMessenger* msg);
	public: bool isWinner();

	private: void generateSystem();
	private: void generateSector(int signPhi, int signTheta, int signMagnitude, int totalCapacity);
	private: Planet generatePlanet(int signPhi, int signTheta, int signMagnitude, int capacity);
	private: void assignInitialPlanets();
	private: double randUnit();
	private: double randAngle();
	private: double distanceSquared(const Planet& a, const Planet& b);

	protected: int* run();
	public: void sendDeployment(int playerId, int sourceId, int destId);
	public: void arriveDeployment(int troops, int dest, int player);
	public: void surrender(GameMessenger* sur, bool wantsConfirm);

	private: void calculateWinner();
	private: void cleanUpGame();
};

#endif