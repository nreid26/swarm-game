#include "Game.h"

#include <string>
#include <stdio.h>      /* printf, NULL */
#include <stdlib.h>     /* srand, rand */
#include <time.h>
#include <cmath>
#include <vector>
#include <unistd.h> //Sleep

#include "GameMessenger.h"
#include "LobbyMessenger.h"
#include "Exception.h"
#include "Thread.h"
#include "Semaphore.h"
#include "ThreadDisposer.h"

#define min(a, b) (a < b) ? a : b

using namespace std;

const int Game::systemRadius = 100;
const int Game::minSeparation = 7;
const double Game::PI = 3.141592654;
const double Game::TROOP_SPEED = (double)2 * systemRadius / 12 / 1000;  // distance units / millisecond
const double Game::CENT_TIME = 30; //Seconds

Game::Game() : guard(0), messenger1(NULL), messenger2(NULL), depCount(0), winner(-1) {
	srand(time(0));
}

Game::~Game() {
	for(int i = 0; i < deployments.size(); i++) { delete deployments[i]; }
}

void Game::addMessenger(GameMessenger* msg) {
	if(messenger1 == NULL) { messenger1 = msg; }
	else if(messenger2 == NULL) { 
		messenger2 = msg;

		generateSystem();

		start();
		guard.signal(); //Let messengers do stuff
	}
}

bool Game::isWinner() { return winner >= 0; }

void Game::generateSystem() {
	for(int a = -1; a < 3; a += 2) { //Generate each octant of the map
		for(int b = -1; b < 3; b += 2) {
			for(int c = -1; c < 3; c += 2) {
				generateSector(a, b, c, 500);
			}
		}
	}

	timeMatrix = vector<vector<int> >(planets.size(), vector<int>(planets.size(), 0)); //Construct the time matrix
	for(int a = 0; a < planets.size(); a++) { //Fill the time matrix
		for(int b = 0; b < planets.size(); b++) {
			timeMatrix[a][b] = sqrt(distanceSquared(planets[a], planets[b])) / TROOP_SPEED;
		}
	}

	assignInitialPlanets();

	for(int i = 0; i < planets.size(); i++) { //Tell each player about the planets
		Planet& p = planets[i];
		messenger1->relayPlanet(p.id, p.x, p.y, p.z, p.capacity, p.growth, p.player, p.troops);
		messenger2->relayPlanet(p.id, p.x, p.y, p.z, p.capacity, p.growth, p.player, p.troops);
	}

	//End world building
	messenger1->relayTerminator(systemRadius, minSeparation);
	messenger2->relayTerminator(systemRadius, minSeparation);
}

void Game::assignInitialPlanets() {
	int cap = 100;
	vector<int> applicable;

	while(applicable.size() < 2) { //Collect the planets of one capacity until there are at least 2
		for(int i = 0; i < planets.size(); i++) {
			if(i == 0) { applicable.clear(); }

			if(planets[i].capacity == cap) { applicable.push_back(i); }
		}
		cap += 100;
	}

	int max = 0, x, y;
	for(auto i = applicable.begin(); i != applicable.end(); i++) { //Iterate over the numbers in applicable
		for(auto j = i; j != applicable.end(); j++) {
			if(timeMatrix[*i][*j] > max) {
				max = timeMatrix[*i][*j];
				x = *i;
				y = *j;
			}
		}
	}

	planets[x].troops = cap;
	planets[y].troops = cap;

	planets[x].player = messenger1->playerId();
	planets[y].player = messenger2->playerId();
}

void Game::generateSector(int signPhi, int signTheta, int signMagnitude, int totalCapacity) {
	while(totalCapacity > 0) {
		int capacity =  ( (int)((totalCapacity / 100 - 1) * randUnit()) /*int between 0 and cap/100 - 1*/ + 1) * 100; //Some fraction of total capacity in hundreds
		totalCapacity -= capacity; //Reduce remaning capacity in this slice

		//Create, id, and push the new planet
		Planet p = generatePlanet(signPhi, signTheta, signMagnitude, capacity);
		p.id = planets.size();
		planets.push_back(p);
	}
}

Game::Planet Game::generatePlanet(int signPhi, int signTheta, int signMagnitude, int capacity) {
	Planet p;
	p.troops = 15; //All planets start with 50 troops
	p.player = -1; //All planets are neutral on creation
	p.capacity = capacity;  //Set the growth of the planet from 100 * (1..5)
	p.growth = 100 / CENT_TIME; //Troops per second

	//Assign and check positions until a valid one is found
	while(true) {
		double phi = randAngle() * signPhi;
		double theta = randAngle() * signTheta;
		double mag = systemRadius * signMagnitude * (1 - randUnit() * randUnit()); //Random radius within systemRadius for planet favouring more distant

		p.z = sin(phi) * mag;
		p.y = cos(phi) * sin(theta) * mag;
		p.x = cos(phi) * cos(theta) * mag;

		int i;
		for(i = 0; i < planets.size() && distanceSquared(planets[i], p) >= minSeparation * minSeparation; i++) {} //Loop over planets until none left or one too close
		if(i == planets.size()) { break; } //i counted over all planets (all passed)
	}

	return p;
}

double Game::randUnit() { return (double)(rand() % 100001) / 100000; }

double Game::randAngle() { return  PI / 2 * randUnit(); } //Random angle 0..PI/2, signed

double Game::distanceSquared(const Planet& a, const Planet& b) { return pow(a.x - b.x, 2) + pow(a.y - b.y, 2) + pow(a.z - b.z, 2); } //Distance between two planets

int* Game::run() { //Conduct troop growth every 100ms
	guard.wait();

		for(int i = 0; i < planets.size(); i++) {
			Planet& p = planets[i];
			if(p.player >= 0) { p.troops += p.growth;} //Only increment players' planets
			p.troops = min(p.troops, p.capacity); //Ensure troops never exceeds capacity
		}

		calculateWinner(); //Test for and declare winner
		if(isWinner()) {
			messenger1->relayWinner(winner); //Release the messangers
			messenger2->relayWinner(winner);

			cancel(); //Clean up the thread
		}
	guard.signal();

	usleep(1000 * 1000); //Sleep for 1 second

	return NULL;
}

void Game::sendDeployment(int playerId, int sourceId, int destId) {
	guard.wait();
		if(isWinner()) { return; } //If someone has won do nothing
		if(planets[sourceId].player != playerId) { return; } //If the player does not own the source planet do nothing

		//Take half the troops from the source planet
		int troops = planets[sourceId].troops / 2;
		planets[sourceId].troops -= troops;

		//Create a deployment thread
		depCount++;
		deployments.push_back(new Deployment(playerId, troops, timeMatrix[sourceId][destId], destId, this));
	guard.signal();

	//Tell the players about this deployment
	messenger1->relayDeployment(sourceId, destId, playerId, troops, timeMatrix[sourceId][destId]);
	messenger2->relayDeployment(sourceId, destId, playerId, troops, timeMatrix[sourceId][destId]);
}

void Game::arriveDeployment(int troops, int dest, int player) {
	guard.wait();
		if(isWinner()) { return; } //If someone has won do nothing

		depCount--; //Discount this dep
		Planet& p = planets[dest];

		if(p.player == player) { p.troops += troops; } //If the player owns the planet
		else {
			if(p.troops > troops) { p.troops -= troops; } //If the planet has more enemies
			else if(p.troops == troops) { //If the invasion is equal to the stationed
				p.troops = 0;
				p.player = -1;
			}
			else { //If the planet has fewer enemies
				p.troops = troops - p.troops;
				p.player = player;
			}
		}
		p.troops = min(p.troops, p.capacity); //Don't exceed planet capacity

		//Update the state of that planet on the client side
		messenger1->relayUpdate(dest, p.player, p.troops);
		messenger2->relayUpdate(dest, p.player, p.troops);
	guard.signal();
}

void Game::surrender(GameMessenger* sur, bool wantsConfirm) {
	guard.wait();
		if(isWinner()) { return; } //If someone has won do nothing

		GameMessenger* win = (messenger1 == sur) ? messenger2 : messenger1;
		winner = win->playerId();

		win->relayWinner(winner);
		if(wantsConfirm) { sur->relayWinner(winner); }
		
		cancel();
	guard.signal();
}

void Game::calculateWinner() { //Is there a winner
	if(depCount > 0) { return; } //No one can win with troops in flight

	//Count the planets owned by each player
	int p1(0), p2(0);
	for(int i = 0; i < planets.size(); i++) {
		if(planets[i].player == messenger1->playerId()) { p1++; }
		else if(planets[i].player == messenger2->playerId()) { p2++; }
	}

	//If a player owns none, return the id of the other
	if(p1 == 0) { winner = messenger2->playerId(); }
	else if(p2 == 0) { winner = messenger1->playerId(); }
}	

