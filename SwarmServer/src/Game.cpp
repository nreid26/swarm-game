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

const int Game::radius = 100;
const int Game::minSeparation = 7;
const double Game::PI = 3.141592654;
const double Game::TROOP_SPEED = 2 * radius / 10;  //units / seconds

Game::Game() : guard(0), startGuard(1), messenger1(NULL), messenger2(NULL), depCount(0), winner(-1) {
	srand(time(0));
}

Game::~Game() {
	for(int i = 0; i < deployments.size(); i++) { delete deployments[i]; }
}

void Game::addMessenger(GameMessenger* msg) {
	startGuard.wait();
		if(messenger1 == NULL) { messenger1 = msg; }
		else if(messenger2 == NULL) { 
			messenger2 = msg;

			generate();

			start();
			guard.signal();
		}
	startGuard.signal();
}

bool Game::isWinner() { return winner >= 0; }

void Game::generate() {
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
			timeMatrix[a][b] = (int)( sqrt(distanceSquared(planets[a], planets[b])) / TROOP_SPEED * 1000);
		}
	}
}

void Game::generateSector(double sp, double st, int mag, int sum) {
	Planet p;

	p.player = -1;
	p.troops = 50;

	while(sum > 0) {
		p.growth = 2 * ((int)(4 * randUnit()) + 1);  //Set the growth of the planet from 2 * (1..5)
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
		timeMatrix.push_back(vector<int>());
		for(int i = 0; i < timeMatrix.size(); i++) { timeMatrix[i].push_back(0); }

		p.id = planets.size();
		planets.push_back(p); //Copy the planet into the vector
		messenger1->relayPlanet(p.id, p.x, p.y, p.z, p.capacity, p.growth, p.player, p.troops);
		messenger2->relayPlanet(p.id, p.x, p.y, p.z, p.capacity, p.growth, p.player, p.troops);
	}
}

double Game::randUnit() { //Random number 0..1
	return (rand() % 1000 + 1) / 1000.0;
}

double Game::randAngle(double sign) { return  PI / 2 * randUnit() * sign; } //Random angle 0..PI/2, signed

double Game::distanceSquared(const Planet& a, const Planet& b) { return pow(a.x - b.x, 2) + pow(a.y - b.y, 2) + pow(a.z - b.z, 2); } //Distance between two planets

int* Game::run() { //Conduct troop growth every 100ms
	guard.wait();

		for(int i = 0; i < planets.size(); i++) {
			if(planets[i].player >= 0) { planets[i].troops += planets[i].growth;} //Only increment players' planets
			planets[i].troops = min(planets[i].troops, planets[i].capacity); //Ensure troops never exceeds capacity
		}

	guard.signal();

	usleep(100 * 1000);

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
		Planet& p(planets[dest]);

		if(p.player == player) { p.troops += troops; } //If the player owns the planet
		else if(p.troops >= troops) { p.troops -= troops;} //If the planet has more enemies
		else { //If the planet has fewer enemies
			p.troops = troops - p.troops;
			p.player = player;
		}
		p.troops = min(p.troops, p.capacity); //Don't exceed planet capacity

		testWinner(); //Test for and declare winner
		if(isWinner()) { 
			messenger1->relayWinner(winner);
			messenger2->relayWinner(winner);
			cancel(); 
		}
		else { //Update the state of that planet on the client side
			messenger1->relayUpdate(dest, p.player, p.troops);
			messenger2->relayUpdate(dest, p.player, p.troops);
		}
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

void Game::testWinner() { //Is there a winner
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

void Game::cancel() { //Specialized cleanup
	ThreadDisposer::getInstance().add(this);
	ThreadBase::cancel();
}