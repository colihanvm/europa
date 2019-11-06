/* Team Bandcamp
 * Class function:
 * The class that handles Chest operations
 */
#include "include/chest.h"
#include "include/pausemenu.h"

/*
 SpriteSheet sheet;
 Animation* anim;
 SDL_Rect chestRect; //The Collision Box
 static int totalInstance = 0;//How many instances of the object exist?
 int toggleButton; //What state is the "Chest open button"? 0 - not pressed  1 - pressed 1st frame 2 - held
 int instanceNumber = 0;
 int state = 0; //State of the Chest
 int x_pos, y_pos; //X and Y according to the TILEMAP
 */

 /*
	0 - Closed
	1 - Opening
	2 - Open
 */

static int totalInstance = 0; //How many instances of the object exist?

//Constructor
//X and Y are in terms of the TILE MAP
Chest::Chest(int x, int y) {
    x_pos = x;
    y_pos = y;
    chestRect = {x*TILE_SIZE, y*TILE_SIZE, TILE_SIZE, TILE_SIZE};
    artifactImgRect = {x*TILE_SIZE, y*TILE_SIZE+3, TILE_SIZE, TILE_SIZE};
    totalInstance++; //Increase instance Number
    instanceNumber = totalInstance;
}

//Deconstructor
Chest::~Chest() {}

Chest::Chest(){}

void Chest::input(const Uint8* keystate)
{
    if(keystate[SDL_SCANCODE_Z])
    {
        if(toggleButton >= 1)
            toggleButton = 2;//Held
        else
            toggleButton = 1; //First Frame Pressed
    }
    else
        toggleButton = 0;  //Not pressed
}

std::string Chest::getInstanceName(){
    std::ostringstream ss;
    ss << instanceNumber;
    return "Chest-"+ss.str();
}

void Chest::init(SDL_Renderer* gRenderer){
    //set up Chest animations
    setSpriteSheet(utils::loadTexture(gRenderer, "res/chest.png"), 6, 2);
    addAnimation("closed", Animation(sheet.get(0,0)));
    addAnimation("damaged", Animation(sheet.get(0,1)));
    addAnimation("open", Animation(sheet.get(0,2)));
    addAnimation("opening", Animation(sheet.getRow(1)));
    addAnimation("broken", Animation(sheet.get(0,1)));
    setAnimation("closed");	//The chest starts off closed
    
    artifactImg = artifactList.at(instanceNumber % uniqueArtifactNumber)->getImage();
}

void Chest::setSpriteSheet(SDL_Texture* _sheet, int _cols, int _rows) {
    sheet = SpriteSheet(_sheet);
    sheet.setClips(_cols, _rows, 32, 32);
}

void Chest::update(std::unordered_map<std::string, Object*> &objectList, std::vector<std::vector<Tile*>> &grid, Uint32 ticks){
    Player * p;
    
    anim->update(ticks);
    
    switch(state){
            
        case 0: //Closed
        {
            if(bulletCollision(objectList))
            {
                state = 6; //Broken
                setAnimation("broken");
                anim->play();
            }
            else
            {
                if(toggleButton == 1){ //If it's the first frame of the button push.
                    //Find the player in the object queue
                    auto it = objectList.find("player");
                    if (it != objectList.end())
                        p = static_cast<Player*>(it->second);
                    
                    if(checkCanOpen(p)){
                        setAnimation("opening");
                        anim->play();
                        state = 1; //Opening
                    }
                }
            }
        }
            break;
            
        case 1: //Opening
        {
            if(anim->playedOnce){
                setAnimation("open");
                anim->play();
                state = 2; //Wait
            }
        }
            break;
            
        case 2: //Wait
        {
            frameTicks += ticks;
            if(frameTicks > 500)
            {
                state = 3; //Raise
                
                if(inventory[8][4] == -1) //Add to the inventory
                {
                    for(int y1 = 0; y1 < 5; y1++){
                        for(int x1 = 0; x1 < 9; x1++){
                            if(inventory[x1][y1] == -1)
                            {
                                inventory[x1][y1] = instanceNumber % uniqueArtifactNumber;
                                return;
                            }
                        }
                    }
                }
            }
        }
            break;
            
        case 3: //Raise
        {
            frameTicks += ticks;
            if(frameTicks > 2){
                frameTicks = 0;
                artifactImgRect.y--;
                
                if((y_pos*TILE_SIZE)-artifactImgRect.y > 25)
                    state = 4; //Wait
            }
            
        }
            break;
            
        case 4: //Wait
        {
            frameTicks += ticks;
            if(frameTicks > 1000)
                state = 5; //Done
        }
            break;
            
        case 6: //Broken
        case 5: //Done
        {}
            break;
    }
    
}

bool Chest::checkCanOpen(Player*& playerObj){
    SDL_Rect * playerRect = playerObj->getRect();
    int x = playerRect->x + int(playerRect->w/2);
    int y = playerRect->y + int(playerRect->h/2);
    
    return (	//Top and Bottom Collision
            (
            (y <= chestRect.y+(TILE_SIZE*2) && y >= chestRect.y-TILE_SIZE)
            &&
            (x <= chestRect.x+TILE_SIZE && x >= chestRect.x)
            )
            || //Left and Right Collision
            (
            (x <= chestRect.x+(TILE_SIZE*2) && x >= chestRect.x-TILE_SIZE)
            &&
            (y <= chestRect.y+TILE_SIZE && y >= chestRect.y)
            )
            );
}

void Chest::addAnimation(std::string tag, Animation anim) {
    anims[tag] = anim;
}

Animation* Chest::getAnimation(std::string tag) {
    return &anims[tag];
}

void Chest::setAnimation(std::string tag) {
    anim = &anims[tag];
    anim->reset();
}

SDL_Renderer* Chest::draw(SDL_Renderer *renderer, SDL_Rect cam){
    SDL_Rect* dest = new SDL_Rect;
    *dest = chestRect;
    dest->x -= cam.x;
    dest->y -= cam.y;
    SDL_RenderCopy(renderer, sheet.getTexture(), anim->getFrame(), dest);
    if(state == 3 || state == 4){
        SDL_Rect* dest1 = new SDL_Rect;
        *dest1 = artifactImgRect;
        dest1->x -= cam.x;
        dest1->y -= cam.y;
        SDL_SetTextureColorMod(
                               artifactList.at(instanceNumber % uniqueArtifactNumber)->getImage(),
                               artifactList.at(instanceNumber % uniqueArtifactNumber)->getR(),
                               artifactList.at(instanceNumber % uniqueArtifactNumber)->getG(),
                               artifactList.at(instanceNumber % uniqueArtifactNumber)->getB());
        SDL_RenderCopy(renderer, artifactImg, NULL, dest1);
        SDL_RenderCopy(renderer, sheet.getTexture(), sheet.get(0,3), dest);
    }
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
    return renderer;
}

bool Chest::bulletCollision(std::unordered_map<std::string, Object*> &objectList){
    std::unordered_map<std::string, Object*>::iterator it;
    for(it = objectList.begin(); it != objectList.end(); it++) {
        if(it->second->getInstanceName().find("proj") != -1) {
            Projectile* temp = (Projectile*)it->second;
            if (collision::checkCol(chestRect, *(temp->getRect()))) {
                temp->projUsed = true; //Delete bullet
                return true;
            }
        }
    }
    
    return false;
}


SDL_Rect* Chest::getRect(){return &chestRect;}

bool Chest::isUsed(){return false;}
