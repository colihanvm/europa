/* Team Bandcamp
 * Class function: Contains core game funtions
 *
 */

#include "include/game.h"

//int SCREEN_WIDTH = 800;
//int SCREEN_HEIGHT = 600;
//int TILE_SIZE = 32;
Mix_Music *bgm;
Mix_Chunk *fire;
//change to true to display credits
bool run_in_credits = false;
//change to ture to display music debugging output
bool music_debug = true;
//used to change through tracks of the song
int track_counter = 0;
//Starts new game
Game::Game() {
    gWindow = nullptr;
    gRenderer = nullptr;
    running = false;
    GSM * gsm;
}

/* Called from Main
 * Set up the game
 */
bool Game::init() {
    
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        std::cout << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }
    //
    if(!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1")) {
        std::cout << "Warning: Linear texture filtering not enabled!" << std::endl;
    }
    
    /* Creating the window
     * Arguments:
     *   Window title
     *   x position (we don't care in this example)
     *   y position (we don't care in this example)
     *   window width
     *   window height
     *   flags, see API for possible flags
     */
    gWindow = SDL_CreateWindow("Bandcamp", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (gWindow == nullptr) {
        std::cout << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        return  false;
    }
    
    // Adding VSync to avoid absurd framerates
    gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (gRenderer == nullptr) {
        std::cout << "Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        return  false;
    }
    //Initialize PNG loading
    int imgFlags = IMG_INIT_PNG;
    if( !( IMG_Init( imgFlags ) & imgFlags ) )
    {
        std::cout << "SDL_image could not initialize! SDL_image Error: " << IMG_GetError() << std::endl;
        return false;
    }
    if( TTF_Init() == -1 )
    {
        std::cout << "SDL_ttf could not initialize! SDL_ttf Error: " << TTF_GetError() << std::endl;
        return false;
    }
    // Set renderer draw/clear color
    SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0x00, 0xFF);
    
    
    //Start the GSM
    
    gsm = new GSM();
    
    gsm->init(gRenderer);
    
    // Initialization Successful
    running = true;
    return true;
}//end Game::init

// Game
void Game::update(Uint32 ticks) {
    gsm->update(ticks);
}//end Game::update

// Updates image displayed to user
void Game::draw() {
    //Clear the Screen
    //SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0x00, 0xFF);
    SDL_RenderClear(gRenderer);
    
    //Draw the current Screen
    gRenderer = gsm->draw(gRenderer);
    SDL_RenderPresent(gRenderer);
}//end Game::draw

/* keystate = a pointer to an array of key states.  1 means key depressed, 0 means not.
 * Processes the updare
 */
void Game::input(const Uint8* keystate){
    gsm->input(keystate);
}//end Game::input

//main game loop
void Game::run() {
    //event handler
    SDL_Event e;
    std::cout << "Running" << std::endl;
    //timer
    Uint32 last_time = SDL_GetTicks();
    Uint32 cur_time = 0;
    Uint32 ticks = 0;
    
    //Music logic
    if(Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
        std::cout << "ERROR" << Mix_GetError() << std::endl;
    //set a default for now ;)
    bgm = Mix_LoadMUS("music/Leftfield Worm.wav");
    
    if(music_debug)
        std::cout << "Loaidng Background music Successful" << std::endl;
    fire = Mix_LoadWAV("music/Soundfx/pew.wav");
    if(music_debug)
        std::cout << "Loaidng Fire Sfx Successful" << std::endl;
    //play music
    Mix_PlayMusic(bgm, -1);
    
    //main loop
    while(running) {
        //handle events on queue
        while(SDL_PollEvent(&e) != 0) {
            //user requests quit
            if(e.type == SDL_QUIT) {
                running = false;
            }
            else if(e.type == SDL_KEYDOWN)// pressed key for music
            {
                switch(e.key.keysym.sym)
                {
                    case SDLK_SPACE: //play pew; player fires gun
                        Mix_PlayChannel(-1, fire, 0);
                        break;
                    case SDLK_p: // play or resume music
                        if(!Mix_PlayingMusic())
                            Mix_PlayMusic(bgm, -1);
                        else if(Mix_PausedMusic())
                            Mix_ResumeMusic();
                        else
                            Mix_PauseMusic();
                        break;
                    case SDLK_n: // load a new track by pressing n
                        Mix_HaltMusic();
                        change_track();
                        break;
                }//end switch(e.key.keysym.sym)
                
            }//end else if(e.type == SDL_KEYDOWN)
            
        }//end while(SDL_PollEvent(&e) != 0)
        
        // keystate = a pointer to an array of key states.  1 means key depressed, 0 means not.
        const Uint8* keystate = SDL_GetKeyboardState( NULL );		// Should this be declared in constructor?
        cur_time = SDL_GetTicks();
        ticks = cur_time - last_time;
        input(keystate);
        update(ticks);
        draw();
        last_time = cur_time;
    }//end while(running)
    
    
    
    //credits
    if(run_in_credits)
    {
        Credits creds = Credits(gRenderer);
        creds.load();
        creds.play();
    }//end if(run_in_credits)
    
    // Tear down and end.  Returns to main
    close();
}//end Game::run

// changes the track that is currently being played
// press n to change current song
void Game::change_track()
{
    switch(track_counter)
    {
        case 0:
            bgm = Mix_LoadMUS("music/CS1666 Game Music 6 120bpm C Minor.wav");
            Mix_PlayMusic(bgm, -1);
            track_counter = 1;
            if(music_debug)
                std::cout << "Track selection - New Territory" << std::endl;
            break;
        case 1:
            bgm = Mix_LoadMUS("music/CS1666 Game Music 2a 128bpm Cm.wav");
            Mix_PlayMusic(bgm, -1);
            track_counter = 2;
            if(music_debug)
                std::cout << "Track selection - Crystalize" << std::endl;
            break;
        case 2:
            bgm = Mix_LoadMUS("music/CS1666 Game Music 2b 128bpm Cm.wav");
            Mix_PlayMusic(bgm, -1);
            track_counter = 3;
            if(music_debug)
                std::cout << "Track selection - Space Palace" << std::endl;
            break;
        case 3:
            bgm = Mix_LoadMUS("music/CS1666 Game Music 3 90bpm Cm.wav");
            Mix_PlayMusic(bgm, -1);
            track_counter = 4;
            if(music_debug)
                std::cout << "Track selection - Subterrianian Liar" << std::endl;
            break;
        case 4:
            bgm = Mix_LoadMUS("music/New Territory/Time 4 Exploration.wav");
            Mix_PlayMusic(bgm, -1);
            track_counter = 5;
            if(music_debug)
                std::cout << "Track selection - Time 4 Exploration" << std::endl;
            
            break;
        case 5:
            bgm = Mix_LoadMUS("music/CS1666 Game Music 5 120bpm Cm.wav");
            Mix_PlayMusic(bgm, -1);
            track_counter = 6;
            if(music_debug)
                std::cout << "Track selection - Space Jam Vibes" << std::endl;
            break;
        case 6:
            bgm = Mix_LoadMUS("music/Throbbing Beat.wav");
            Mix_PlayMusic(bgm, -1);
            track_counter = 7;
            if(music_debug)
                std::cout << "Track selection - Throbbing Beat" << std::endl;
            break;
        case 7:
            bgm = Mix_LoadMUS("music/Leftfield Worm.wav");
            Mix_PlayMusic(bgm, -1);
            track_counter = 0;
            if(music_debug)
                std::cout << "Track selection - Leftfield Worm" << std::endl;	                        	
            break;
        default: //title; something went wrong
            bgm = Mix_LoadMUS("music/New Territory/Theme.wav");
            Mix_PlayMusic(bgm, -1);                        		
            std::cout << "Track selection ERROR" << std::endl;
            break;
    }//end switch(track_counter)	
    
}//end Game:change_track

// free memory and quit
// Returns main after completion
void Game::close() {
    
    SDL_DestroyRenderer(gRenderer);
    SDL_DestroyWindow(gWindow);
    Mix_FreeMusic(bgm);
    Mix_FreeChunk(fire);
    
    gRenderer = nullptr;
    gWindow = nullptr;
    bgm = nullptr;
    fire = nullptr;
    
    // Quit SDL subsystems
    Mix_Quit();
    IMG_Quit();
    SDL_Quit();
}//end Game::close
