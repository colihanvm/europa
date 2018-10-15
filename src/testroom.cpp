#include <SDL.h>
#include <algorithm>
#include <string>

#include "include/object.h"
#include "include/player.h"
#include "include/HUD.h"
#include "include/testroom.h"
#include "include/game.h"
HUD h;
Player p;
Pickup currentP;

TestRoom::TestRoom(int* roomNumber){
	start = false;
	std::vector<Object*> objectList;
	roomReference = roomNumber;
	
}

void TestRoom::init(SDL_Renderer* reference){
	rendererReference = reference;
	SDL_Rect player_box = {0, 0, tile_s, tile_s};
	p = Player(player_box);
	
	
	h.init(reference);
	p.init(reference);
	
	
	//Player and HUD in the Room
	objectList.push_back(&h);
	objectList.push_back(&p);
}

void TestRoom::update(){
	//Should we add a pickup?
	checkPickups();
	for(int i=0; i < objectList.size(); i++){
		objectList[i]->update(objectList);
	}
}

void TestRoom::checkPickups(){
	if (!(std::find(objectList.begin(), objectList.end(), &currentP) != objectList.end())) {
		
		int pickupX = rand()%(screen_w-tile_s);
		int pickupY = rand()%(screen_h-tile_s);
		SDL_Rect pickupBox = {pickupX, pickupY, tile_s, tile_s};
		
		int pickupValue = rand()%25+25;
		
		int pickupType = rand()%2;
		char type;
		
		if (pickupType == 0)
			type = 'e';
		else
			type = 'o';
		
		currentP = Pickup(pickupBox, type, pickupValue, &p, &h);
		objectList.push_back(&currentP);
	}
}

void TestRoom::input(const Uint8* keystate){
	for(int i=0; i < objectList.size(); i++){
		objectList[i]->input(keystate);
	}
}

SDL_Renderer* TestRoom::draw(SDL_Renderer *renderer){
	for(int i=0; i < objectList.size(); i++){	
		renderer = objectList[i]->draw(renderer);
	}
	return renderer;
}