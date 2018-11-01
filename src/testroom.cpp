/* Team Bandcamp
 * Class function: 
 * 
*/

#include <SDL.h>
#include <algorithm>
#include <string>

#include "include/object.h"
#include "include/player.h"
#include "include/spritesheet.h"
#include "include/HUD.h"
#include "include/testroom.h"
#include "include/game.h"
#include "include/ooze.h"
#include "include/circle.h"
#include "include/collision.h"
#include "include/pickup.h"

constexpr int UPDATE_MAX = 100;
int updateCount = 1;
int oldTemp = 100;
int oldO2 = 100;
int oldAte = 0;

bool spawnPickup = true;
// Heads up display 
HUD h;
Player p;

bool pauseB, enterHeld; //Have we pushed the pauseButton this frame?

TestRoom::TestRoom() : Screen(){} //from merge

Ooze o;
SDL_Rect leftWall;
SDL_Rect rightWall;
SDL_Rect upperWall;
Circle centerPillar;


// ADD COMMENTS 
void TestRoom::init(SDL_Renderer* reference){
	rendererReference = reference;
	SDL_Rect player_box = {screen_w/2, screen_h/2, tile_s, tile_s};
	p = Player(player_box);
	SDL_Rect ooze_box = {screen_w/2, 3*screen_h/8, 30, 30};
	o = Ooze(ooze_box, &p, &h);
    
	h.init(reference);
	p.init(reference);
	o.init(reference);
	
	//Player and HUD in the Room
	objectList["player"] = &p;
	objectList["hud"] = &h;
	// Change to add ooze to list as initialized
	objectList["ooze"] = &o;

	//Init walls in the room
	leftWall = {screen_w/4, screen_h/4, screen_w/12, screen_h/2};
	rightWall = {screen_w/4 * 3 - screen_w/12, screen_h/4, screen_w/12, screen_h/2};
	upperWall = {screen_w/4, screen_h/4, screen_w/2, screen_h/12};
	centerPillar = {screen_w/2, screen_h/2 + (tile_s * 5), tile_s};
}

// ADD COMMENTS 
void TestRoom::update(Uint32 ticks){
	if(pauseB)
	{ //If you set the currentScreen in the Input method it will cause an array out of bounds error.
		pauseB = false;
		enterHeld = true;
		currentScreen = -1;//The Pause Command  <- Its an arbitrary number.
	}
	

	if (spawnPickup) movePickup(rendererReference); //new way of deciding when to spawn pickup
	// TODO: better way to check for pickup being consumed?
	/*if (h.currentTemp > oldTemp || h.currentOxygen > oldO2 || o.getAte() > oldAte) movePickup(rendererReference);
	oldTemp = h.currentTemp;
	oldO2 = h.currentOxygen;
	oldAte = o.getAte();*/

	std::unordered_map<std::string, Object*>::iterator it = objectList.begin();
	while(it != objectList.end()){
		it->second->update(&objectList, ticks);
		if(it->second->isUsed()) {
			it = objectList.erase(it);
		}
		it++;
	}
	if (updateCount == 0) {
		h.currentTemp = std::max(0, h.currentTemp-5);
		h.currentOxygen = std::max(0, h.currentOxygen-5);
		if (h.currentTemp == 0) {
			h.currentHealth = std::max(0, h.currentHealth-5);
		}
		if (h.currentOxygen == 0) {
			h.currentHealth = std::max(0, h.currentHealth-5);
		}
	}
	updateCount = (updateCount+1)%UPDATE_MAX;
}

// ADD COMMENTS 
void TestRoom::movePickup(SDL_Renderer* reference) {
	int pickupX = std::max(tile_s, rand()%(screen_w-tile_s));
	int pickupY = std::max(tile_s, rand()%(screen_h-tile_s));
	SDL_Rect pickupBox = {pickupX, pickupY, tile_s, tile_s};
	
	if(collision::checkCol(pickupBox, leftWall) 
		|| collision::checkCol(pickupBox, rightWall)
		|| collision::checkCol(pickupBox, upperWall)
		|| collision::checkCol(pickupBox, centerPillar))
	{
		movePickup(reference);
	}
	else
	{
		int pickupValue = rand()%25+25;
	
		int pickupType = rand()%2;
		char type;
		
		if (pickupType == 0)
			type = 'e';
		else
			type = 'o';
		
		Pickup *newP  = new Pickup(pickupBox, type, pickupValue, &p, &h);
		objectList[newP->getInstanceName()] = newP;
		newP->init(reference);
		spawnPickup = false; //don't need a new pickup; one was just made
	}
}

// used to allow other objects to tell testroom to spawn a pickup
void TestRoom::setSpawnPickup(bool set) {
	spawnPickup = set;
}
// ADD COMMENTS 
void TestRoom::input(const Uint8* keystate){
	//If you push the pause button
	
	//When you come back into the room after a pause, you will most likely still be holding down
	//the enter key. This prevents you from going straight back into the pause menu.
	if(enterHeld && keystate[SDL_SCANCODE_RETURN])
		pauseB = false;
	else
	{
		enterHeld = false;
		pauseB = keystate[SDL_SCANCODE_RETURN];
		
		std::unordered_map<std::string, Object*>::iterator it = objectList.begin();
		while(it != objectList.end()){
			it->second->input(keystate);
			it++;
		}
	}
}

// ADD COMMENTS 
SDL_Renderer* TestRoom::draw(SDL_Renderer *renderer){
	std::unordered_map<std::string, Object*>::iterator it = objectList.begin();
	while(it != objectList.end()){
		renderer = it->second->draw(renderer);
		it++;
	}

	//Draws walls in the room
	SDL_SetRenderDrawColor(renderer, 0, 0xFF, 0, 0);
	SDL_RenderFillRect(renderer, &leftWall);
	SDL_RenderFillRect(renderer, &rightWall);
	SDL_RenderFillRect(renderer, &upperWall);
	centerPillar.drawCircle(renderer);

	return renderer;
}
