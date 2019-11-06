#ifndef BANDCAMP_CHEST_H_
#define BANDCAMP_CHEST_H_

// forward declarations
/*
 #include <SDL.h>
 int uniqueArtifactNumber;
 vector<Artifact*> artifactList; //The list of unique artifacts in the game.
 int* inventory[9][5];
 */
/*class PauseMenu {
     std::vector<Artifact*> artifactList; //The list of unique artifacts in the game.
     int inventory[9][5];
     int uniqueArtifactNumber; //How many unique artifacts are in the game?
};*/


// included dependencies
#include "game.h"
#include "utils.h"
#include "spritesheet.h"
#include "animation.h"
#include "object.h"

// class declarations
class Chest : public Object
{
private:
    SpriteSheet sheet;
    SDL_Texture* artifactImg;
    std::unordered_map<std::string, Animation> anims;
    Animation* anim;
    SDL_Rect chestRect; //The Collision Box
    SDL_Rect artifactImgRect;
    int toggleButton; //What state is the "Chest open button"? 0 - not pressed  1 - pressed 1st frame 2 - held
    int instanceNumber = 0;
    int x_pos, y_pos; //X and Y according to the TILEMAP
    int state = 0; //State of the Chest
    int frameTicks; //For animations
    
public:
    //static int totalInstance;
    void input(const Uint8* keystate);
    void init(SDL_Renderer *renderer);
    void update(std::unordered_map<std::string, Object*> &objectList, std::vector<std::vector<Tile*>> &grid, Uint32 ticks);
    std::string getInstanceName();
    SDL_Renderer* draw(SDL_Renderer *renderer, SDL_Rect cam);
    Chest(int x, int y);
    ~Chest();
    Chest();
    void checkOverlap(std::unordered_map<std::string, Object*> *objectList);
    void setSpriteSheet(SDL_Texture* _sheet, int _cols, int _rows);
    bool checkCanOpen(Player*& playerObj);
    void addAnimation(std::string tag, Animation anim);
    Animation* getAnimation(std::string tag);
    void setAnimation(std::string tag);
    bool isUsed();
    bool bulletCollision(std::unordered_map<std::string, Object*> &objectList);
    SDL_Rect* getRect();
};

#endif  //  BANDCAMP_CHEST_H_
