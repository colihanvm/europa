#ifndef BANDCAMP_PROJECTILE_H_
#define BANDCAMP_PROJECTILE_H_

#include <algorithm>
#include <SDL.h>
#include <string>
#include <unordered_map>
#include <vector>

#include "collision.h"
#include "object.h"
#include "testroom.h"
#include "utils.h"

class Projectile : public Object
{
	private:
		char projType;
		SDL_Rect projRect; //The Collision Box
		bool up;
	    bool down;
	    bool left;
	    bool right;
		int playerXVel;
		int playerYVel;
        bool projUsed;
        //Image Stuff
        SDL_Texture* projImg;
        SDL_Rect projImgRect;
        SDL_Rect projDrawBox;	//Where the Image is drawn on screen
        Uint32 projTicks;

	public:
		void input(const Uint8* keystate);
		void init(SDL_Renderer *renderer);
		void update(std::unordered_map<std::string, Object*> *objectList, std::vector<std::vector<int>> grid, Uint32 ticks);
		std::string getInstanceName();
		SDL_Renderer* draw(SDL_Renderer *renderer, SDL_Rect cam);
		Projectile(char type, int playerX, int playerY);
        ~Projectile();
		Projectile();
		void checkProjOverlap(int curX, int curY, std::vector<std::vector<int>> grid);
		SDL_Rect* getProjRect();
		void updatePosition(Uint32 ticks);
		bool isUsed();
};

#endif  //  BANDCAMP_PROJECTILE_H_
