#ifndef BANDCAMP_HUD_H_
#define BANDCAMP_HUD_H_

#include <iostream>
#include <string>
#include <vector>
#include <SDL.h>
#include <SDL_image.h>
#include "utils.h"
#include "object.h"

//initialize the HUD
class HUD : public Object
{
	public:
		HUD();
		~HUD();
		void init(SDL_Renderer* _renderer);
		void update(std::vector<Object*> objectList, Uint32 ticks);
		void input(const Uint8* keystate);
		SDL_Renderer* change_levels(SDL_Renderer* _renderer, int oxygen_level, int temperature_level, int health_level);
		SDL_Renderer* draw(SDL_Renderer* gRenderer);
		int currentTemp;
		int currentOxygen;
		int currentHealth;
	private:
		bool init_h;
		std::vector<SDL_Texture*> hud;
		SDL_Renderer* renderer_h;
		SDL_Rect Temp;
		SDL_Rect Oxygen;
};

#endif  //  BANDCAMP_HUD_H_