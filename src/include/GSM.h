#ifndef BANDCAMP_GSM_H_
#define BANDCAMP_GSM_H_

#include <SDL.h>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>
#include <SDL_image.h>

#include "screen.h"
#include "menu.h"
#include "randommap.h"
#include "death.h"
#include "PauseMenu.h"
#include "credits.h"
#include "testtransition_1.h"

class GSM {
private:
    std::vector<Screen*> roomList;
    int previousScreen; //To check if we changed rooms
    bool running;
    SDL_Renderer* rendererReference;
    Menu * testMenu;
public:
    
    GSM();
    //~GSM();
    static int currentScreen;
    static void setCurrentScreen(int newScreen);
    void init(SDL_Renderer *renderer);
    void input(const Uint8* keystate);
    void update(Uint32 ticks);
    SDL_Renderer* draw(SDL_Renderer *renderer);
    
};
#endif	//	BANDCAMP_GSM_H_
