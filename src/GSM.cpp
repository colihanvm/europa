/* Team Bandcamp
 * Class function: Game state manger
 *
 */

#include "include/GSM.h"

RandomMap randomMap;
TestTransition_1 testTransitionScreen;
PauseMenu pauseMenu;
Death deathScreen;
bool pause;	//Is the game paused
int tempScreen; //What was the room before you paused?

int GSM::currentScreen = 0;	// Static variable indicating current screen


// Game state manager
GSM::GSM(){
    
    //Init Screens
    //They all get passed the pointer to the
    //Current Screen so they can change it when they see fit.
    
    
    testMenu = new Menu();
    roomList.push_back(testMenu);
    //testScreen = TestRoom();
    //roomList.push_back(&testScreen);
    testTransitionScreen = TestTransition_1();
    roomList.push_back(&testTransitionScreen);
    randomMap = RandomMap();
    roomList.push_back(&randomMap);
    pauseMenu = PauseMenu();
    roomList.push_back(&pauseMenu);
    deathScreen = Death();
    roomList.push_back(&deathScreen);
    
    previousScreen = 0;
    pause = false;
    running = false;		// does this refer to the game running bool? its own from GSM.h
}

// Initialize GSM
void GSM::init(SDL_Renderer* reference){
    std::cout << "Init GSM" << std::endl;
    //When Objects need to load their sprites, they will be passed this renderer
    //As a reference for their init method.
    rendererReference = reference;
    
    roomList[currentScreen]->init(reference);
    pauseMenu.init(rendererReference); //So that the artifact list can be generated.
    running = true;
}

// Update the GSM state
void GSM::update(Uint32 ticks){
    previousScreen = currentScreen;
    
    if(pause)
        pauseMenu.update(ticks);
    else
        roomList[currentScreen]->update(ticks);
    
    //Checking if we changed screens this loop
    //If so, then call the init to the new screen.
    if(previousScreen != currentScreen)
    {
        if(currentScreen == -1){ //The Pause Command
            pause = true;
            tempScreen = previousScreen;
            pauseMenu.init(rendererReference);
        }
        else if(currentScreen == -2){ //The Unpause Command
            pause = false;
            currentScreen = tempScreen;
        }
        else
            roomList[currentScreen]->init(rendererReference);
    }
}

//
SDL_Renderer* GSM::draw(SDL_Renderer *renderer){
    if(pause)
    {
        renderer = roomList[tempScreen]->draw(renderer); //So long as you don't put game logic in your draw method, this should work.
        renderer = pauseMenu.draw(renderer); //Draw the pause menu
    }
    else
        renderer = roomList[currentScreen]->draw(renderer);
    
    return renderer;	
}

// 
void GSM::input(const Uint8* keystate){
    if(pause)
        pauseMenu.input(keystate);
    else
        roomList[currentScreen]->input(keystate);
}
