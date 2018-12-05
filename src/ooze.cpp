#include "include/ooze.h"
#include "include/player.h"

//initialize static member variables
int Ooze::totalOoze = 0;

constexpr int MAX_SPEED = 1;
constexpr int BORDER_SIZE = 32;

// Default Constructor
Ooze::Ooze():state{CLONING}, hostility{0} {}

// Constructor
Ooze::Ooze(Room* room, Tilemap* t):
state{HANGRY},
hostility{0},
tilemap{t}
{
    target = player->getRect();
    curRoom = room;
    neighbors = curRoom->getNeighbors();
    roomRect = curRoom->getRectCopy();
    rect = {((roomRect.x + roomRect.w)/2) * tile_s, ((roomRect.y + roomRect.h)/2) * tile_s, 30, 30};
    totalOoze++; //Increase # of instances counter
	oozeNumber = totalOoze;
	int overlapTicks = 0;
    //Speed
    x_deltav = 0;
    y_deltav = 0;
    x_vel = 1;
    y_vel = 1;

    ate = 1;
    
    // Genetic statistics
    stats.health =      3 ;
    stats.attack =      50; //time delay between ticks damage
    stats.speed =       3 ;
    stats.health_cost = 3 ;
    stats.num_cost =    3 ;
    Mutate();
    
    roomTiles.startTile = nullptr;
    roomTiles.endTile = nullptr;
    
    lastRoom = nullptr;
    initialized = false;
    intersects = curRoom->getIntersects();
    doors = tilemap->getDoors();
    squeeze = false;
    squeezeItr = 0;
    iter = 0;
	used = false;
}

//Other constructor?
//Ooze::Ooze(State st, int hostil) :state{st}, hostility{hostil} {}

// Copy Constructor
Ooze::Ooze(const Ooze& other):
    sheet{other.sheet},
    anim{other.anim},
tilemap{other.tilemap},
target{other.target},
    state{ROAMING}
{
    totalOoze++;
    oozeNumber = totalOoze;
    
    curRoom = other.curRoom;
    SDL_Rect *temp = curRoom->getRect();
    rect = {((temp->x + temp->w)/2) * tile_s, ((temp->y + temp->h)/2) * tile_s, 30, 30};
    
    stats = other.stats;
    Mutate();
    
}

//Destructor
Ooze::~Ooze(){};

std::string Ooze::getInstanceName(){
	std::ostringstream ss;
  ss << oozeNumber;
	return "ooze-"+ss.str();
}

 /*Summary
 * Argument  
 *
 */
void Ooze::input(const Uint8* keystate){}

 /*Summary
 * Argument  
 *
 */
void Ooze::init(SDL_Renderer* gRenderer) {
    texture = utils::loadTexture(gRenderer, "res/ooze.png");
	setSpriteSheet(texture, 3, 1);
    addAnimation("wandering", Animation(getSheet().getRow(0)));
    setAnimation("wandering");
    renderer = gRenderer;
}

 /*Summary
 * Argument  
 *
*/
void Ooze::setSpriteSheet(SDL_Texture* _sheet, int _cols, int _rows) {
    sheet = SpriteSheet(_sheet);
    sheet.setClips(_cols, _rows, rect.w, rect.h);
}

//*******TO DO:
//update motion here
void Ooze::update(std::unordered_map<std::string, Object*> &objectList, std::vector<std::vector<Tile*>> &grid, Uint32 ticks) {
	//std::cout << "Entered Ooze update" << std::endl;
	
	//Checks to make sure our ooze isn't stuck in a wall
    //Must be declared here because we need the grid, but should only run on the
    //first update. Runs very quickly too
    
    ////std::cout << "X " << roomRect.x << " Y " << roomRect.y << " W " << roomRect.h << " H " << roomRect.w << std::endl;

    if(!initialized) {
        initRoom(grid);
        initialized = true;
    } 
    //Get the position of the ooze before it moves
    int x_deltav = 0;
	int y_deltav = 0;

    //Needed for collision detection
    int curX = rect.x;
    int curY = rect.y;

    int pickupX;
    int pickupY;

    //might move order of update calls
    //bool stateChange = updateState(objectList, ticks);
	bool overlap = checkOozeOverlap(objectList, ticks);
    bool los;
	if(!overlap){    
        if(iter % 15 == 0) {
            target = pickTarget(objectList, grid);
//            p = static_cast<Player*>(it->second);
            if (true) {
            //check which direction the target is
            //Only move if we can see the player
                moveLine(grid, target);
            }
        //updateVelocity(x_deltav,y_deltav);
        }
        //If we don't have a line of sight with the player or pickup, check the other room
        else {
            //moveRoom(grid);
        }
        updatePosition();
    }
    //foundFood(getPickup(objectList));
    //update animation
    updateAnimation(ticks);
    //checkBounds(screen_w, screen_h, true);
    //Check you haven't collided with object
    if(!squeeze)
        checkCollision(curX, curY, grid, true);
    std::unordered_map<std::string, Object*>::iterator it;
    for(it = objectList.begin(); it != objectList.end(); it++) {
		if(it->second->getInstanceName().find("proj") != -1) {
			Projectile* temp = (Projectile*)it->second;
			if (collision::checkCol(rect, *(temp->getRect()))) {
				std::cout << "Ooze hit" << std::endl;;
                hurt(1);
				temp->projUsed = true;
				break;
			}
		}
	}
    iter++;
	//std::cout << "Exiting Ooze update" << std::endl;
}

 /*Summary
 * Argument  
 *
 */
void Ooze::increaseHostility() {
	if (hostility < 10)
		hostility++;
}

 /*Summary
 * Argument  
 *
 */
void Ooze::decreaseHostility() {
	if (hostility >  0)
		hostility--;
}

 /*Summary
 * Argument  
 *
 */
SDL_Renderer* Ooze::draw(SDL_Renderer* renderer, SDL_Rect cam) {
    SDL_Rect* dest = new SDL_Rect;
    *dest = rect;
    dest->x -= cam.x;
    dest->y -= cam.y;
    SDL_RenderCopy(renderer, sheet.getTexture(), anim->getFrame(), dest);\
   return renderer;
}

SDL_Rect* Ooze::pickTarget(std::unordered_map<std::string, Object*> &objectList, std::vector<std::vector<Tile*>> &grid) {
    switch(this->state) {
        case CLONING: {
            return nullptr;
        }
        case HANGRY: {
            squeeze = false;
            std::unordered_map<std::string, Object*>::iterator it = objectList.begin();
            while(it != objectList.end()){
                if (!it->first.substr(0,6).compare("Pickup")) {
                    Pickup* temp = (Pickup*)it->second;
                    bool losPickup = drawLine(grid, temp->getRect());
                        
                    if(losPickup){
                        return temp->getRect();
                    }
                    else {
                        bool losPlayer = drawLine(grid, player->getRect());
                        if(losPlayer)
                            return player->getRect();
                        //If we don't have a line of sight with the player or pickup, check the other room
                        else {
                            moveRoom(grid);
                            state = ROOMEXIT;
                            return roomTiles.startTile;   
                        }
                    }
                }
            it++;
            }
        }
        case ROOMEXIT: {
            std::unordered_map<std::string, Object*>::iterator it = objectList.begin();
            while(it != objectList.end()){
                if (!it->first.substr(0,6).compare("Pickup")) {
                    Pickup* temp = (Pickup*)it->second;
                    bool losPickup = drawLine(grid, temp->getRect());
                        
                    if(losPickup) {
                        state = HANGRY;
                        return temp->getRect();
                    }  
                    else {
                        bool losPlayer = drawLine(grid, player->getRect());
                        if(losPlayer) {
                            state = HANGRY;
                            return player->getRect();
                        }
                        //If we don't have a line of sight with the player or pickup, check the other room
                        else if(drawLine(grid, roomTiles.endTile) || squeeze) {
                            state = ROOMENTER;
                            return roomTiles.endTile;
                        }
                        else {
                                squeezeItr++;
                            if(squeezeItr == 15) {
                                squeeze = true;
                                squeezeItr = 0;
                            }
                            return roomTiles.startTile; 
                        }
                    }                  
                }
                it++;
            }
            return nullptr;
        }
        case ROOMENTER: {
            std::unordered_map<std::string, Object*>::iterator it = objectList.begin();
            while(it != objectList.end()){
                if (!it->first.substr(0,6).compare("Pickup")) {
                    Pickup* temp = (Pickup*)it->second;
                    bool losPickup = drawLine(grid, temp->getRect());
                        
                    if(losPickup) {
                        squeeze = false;
                        state = HANGRY;
                        switchRoom();
                        return temp->getRect();
                    }  
                    else {
                        bool losPlayer = drawLine(grid, player->getRect());
                        if(losPlayer) {
                            squeeze = false;
                            state = HANGRY;
                            switchRoom();
                            return player->getRect();
                        }
                        //If we don't have a line of sight with the player or pickup, check the other room
                        else if(collision::checkCol(rect, *roomTiles.endTile)) {
                            state = HANGRY;
                            squeeze = false;
                            switchRoom();

                            return nullptr;
                        }
                        else 
                            return roomTiles.endTile;
                    }                  
                }
                it++;
            }
            return nullptr;
        }
		case DYING:
			used = true;
        default:
            return player->getRect();
    }
    return nullptr;
}

// TODO: combine this with the overlap method below, which
//       checks for overlap betw. ooze and player
bool Ooze::foundFood(Pickup* food) {
    if (food) {
        SDL_Rect* fRect = food->getRect();
        bool overlap = collision::checkCol(rect, *fRect);
        if (overlap) {
            //food->use();
            ate++;
            std::string s = getInstanceName() + " ATE: "+ food->getInstanceName() + ". HAS ATE: " + std::to_string(ate);
            if(ate > 2) {
                RandomMap::setSpawnOoze(true);
                Ooze(*this);
                ate = 0;
            }
            return true;
        }
    }
    return false;
}

/* Summary
 * Argument  
 *
 */
int Ooze::getAte() {
    return ate;
}


OozeState Ooze::getState() {
    return state;
}


bool Ooze::updateState(std::unordered_map<std::string, Object*> &objectList, Uint32 ticks) {
    
    switch(this->state) {
//            //std::cout << "roaming" << std::endl;
        case ROAMING: {
            updateVelocity(utils::uniformDist(), utils::uniformDist());
            if(target){ //if(target == player);
                state = HANGRY;
                return true;
            }
            break;
        }
        case HANGRY: {
//            foundFood(Pickup* food);
            if(!target){
                //std::cout << "ooze" << oozeNumber << ": \"Target Lost...\"" << std::endl;
                state = ROAMING;
            }
            if (ate > 0) {                      // "Time to eat!"
                state = CLONING;
                
                ate = 0;
                return true;
            }
            break;
        }
        case CLONING: {
            //std::cout << "cloning" << std::endl;
//            RandomMap::setSpawnOoze(true);
            Ooze(*this);
            state = ROAMING;
            return true;
            break;
        }
        case FIGHTING: {
            
            if ( stats.health < stats.health_cost || oozeNumber < stats.num_cost ) {
                state = FLEEING;
                return true;
            }
            break;
        }
        case FLEEING: {
            // look for hiding places
            /*if ( next to hiding spot ) {
                state = HIDING;
            } */
            break;
        }
        case HIDING: {
            
            break;
        }
        case DYING: {
            this->~Ooze();
            break;
        }
            
        case ROOMENTER: {
            break;
        }
        case ROOMEXIT: {
            break;
        }
    }
    return false;
}

//Checks if the player overlapped with the ooze and acts accordingly
//based on pickup's method
bool Ooze::checkOozeOverlap(std::unordered_map<std::string, Object*> &objectList, Uint32 ticks) {
	SDL_Rect* pRect = player->getRect();
	bool overlap = collision::checkCol(rect, *pRect);
    if (overlap) {
		overlapTicks += ticks;
		if (overlapTicks > stats.attack) {
			hud_g->currentHealth = std::max(0, hud_g->currentHealth-damage);
			std::string s = "HIT: "+getInstanceName();
			overlapTicks = 0;
		}
	} else {
		overlapTicks = 0;
	}

	player->setEnemy(overlap);

	return overlap;
}

 /*Summary
 * Argument  
 *
*/
void Ooze::updateAnimation(Uint32 ticks) {

    if(true) { //ticks/10%2 == 2
        setAnimation("wandering");
        anim->play();
        ////std::cout << "ooze animating";
    }
    else {
        anim->reset();
        anim->stop();
    }
    anim->update(ticks);
}

 /*Summary
 * Argument  
 *
*/
void Ooze::updatePosition() {
    rect.x += x_vel;
    rect.y += y_vel;
}

 /*Summary
 * Argument  
 *
 */
void Ooze::checkBounds(int max_width, int max_height, bool move) {
    if(move)
    {
        if (rect.x < BORDER_SIZE){
            rect.x = BORDER_SIZE;
            x_vel = -x_vel;
        }
        else if (rect.x + rect.w > max_width - BORDER_SIZE){
            rect.x = max_width - rect.w - BORDER_SIZE;
            x_vel = -x_vel;
        }
        
        if (rect.y < BORDER_SIZE){
            rect.y = BORDER_SIZE;
            y_vel = -y_vel;
        }
        else if (rect.y + rect.h > max_height - BORDER_SIZE){
            rect.y = max_height - rect.h - BORDER_SIZE;
            y_vel = -y_vel;
        }
    }
    else
    {
        if (colRect.x < BORDER_SIZE){
            colRect.x = BORDER_SIZE;
        }
        else if (colRect.x + colRect.w > max_width - BORDER_SIZE){
            colRect.x = max_width - colRect.w - BORDER_SIZE;
        }
        
        if (colRect.y < BORDER_SIZE){
            colRect.y = BORDER_SIZE;
        }
        else if (colRect.y + colRect.h > max_height - BORDER_SIZE){
            colRect.y = max_height - colRect.h - BORDER_SIZE;
        }
    }    
}


/* Summary
 * Argument  
 *
 */
bool Ooze::isUsed() { return used; }

Animation* Ooze::getAnimation(std::string tag) { return &anims[tag]; }

void Ooze::setAnimation(std::string tag) { anim = &anims[tag]; }

void Ooze::addAnimation(std::string tag, Animation anim) { anims[tag] = anim; }

SpriteSheet Ooze::getSheet() { return sheet; }

//returns width
int Ooze::getWidth() { return rect.w; }

//returns height
int Ooze::getHeight() { return rect.h; }

//returns x position
int Ooze::getX() { return rect.x; }

//returns y position
int Ooze::getY() { return rect.y; }

SDL_Rect* Ooze::getRect() { return &rect; }

void Ooze::updateVelocity(int _xdv, int _ydv) {
    
    // If we dont want out dot to be in a frictionless vacuum...
    if (_xdv == 0) {
        // No user-supplied "push", return to rest
        if (x_vel > 0)
            _xdv = -1;
        else if (x_vel < 0)
            _xdv = 1;
    }
    if (_ydv == 0) {
        if (y_vel > 0)
            _ydv = -1;
        else if (y_vel < 0)
            _ydv = 1;
    }
    
    // Speed up/slow down
    x_vel += _xdv;
    y_vel += _ydv;

    // Check speed limits
    if (x_vel < -1 * MAX_SPEED)
        x_vel = -1 * MAX_SPEED;
    else if (x_vel > MAX_SPEED)
        x_vel = MAX_SPEED;

    if (y_vel < -1 * MAX_SPEED)
        y_vel = -1 * MAX_SPEED;
    else if (y_vel > MAX_SPEED)
        y_vel = MAX_SPEED;
}


bool Ooze::checkCollision(int curX, int curY, std::vector<std::vector<Tile*>> &grid, bool move) {
    //Checks the collision of each object and determines where the ooze should stop
    //Also checks to see if ooze has line of sight
    if(move) {
        if(collision::checkColLeft(rect, grid, 32) || collision::checkColRight(rect, grid, 32)) {
            rect.x = curX;
        }
        
        if(collision::checkColTop(rect, grid, 32) || collision::checkColBottom(rect, grid, 32)) {
            rect.y = curY;

            rect.x += x_vel;

            y_vel = 0;
            if(collision::checkColLeft(rect, grid, 32) || collision::checkColRight(rect, grid, 32)) {
                x_vel = 0; 
                rect.x = curX;
            }
        }
    }
    else {
        if(collision::checkColLeft(colRect, grid, 32) || collision::checkColRight(colRect, grid, 32)) 
            return false;
        if(collision::checkColTop(colRect, grid, 32) || collision::checkColBottom(colRect, grid, 32)) 
            return false;
        else 
            return true;
    } 
    return false;       
}


//Uses Bresenham's alg to check to see if we have a line of sight with the player
//This draws the line fully but does NOT move the player at all
bool Ooze::drawLine(std::vector<std::vector<Tile*>> &grid, SDL_Rect* target) {
    int startX = rect.x;
    int startY = rect.y;
    int endX = target->x;
    int endY = target->y;
    colRect = {startX, startY, rect.w, rect.h};
    int slope = 0;
    int xDir = 0;
    int yDir = 0;
    bool sight = false;

    if (target->y > rect.y) 
        yDir = 1;
	if (target->x > rect.x) 
        xDir = 1;
	if (target->y < rect.y) {
        endY = target->y + target->h;
        yDir = -1;
    }
	if (target->x < rect.x) {
        endX = target->x + target->w;
        xDir = -1;
    }
        
    int deltaX = endX - startX;
    int deltaY = endY - startY;
    deltaX = abs(deltaX * 2);
    deltaY = abs(deltaY * 2);

    if(deltaX > deltaY) {
        slope = deltaY * 2 - deltaX;
        while(startX != endX) {
            if(slope >= 0) {
                startY += yDir;
                colRect.y += yDir;
                slope -= deltaX;
            }

            startX += xDir;
            colRect.x += xDir;
            slope += deltaY;
            //checkBounds(screen_w, screen_h, false);
            sight = checkCollision(colRect.x, colRect.y, grid, false);
            if(!sight)
                break;
        }
        return sight;
    }
    else {
        slope = deltaX * 2 - deltaY;
        while(startY != endY) {
            if(slope >= 0) {
                startX += xDir;
                colRect.x += xDir;
                slope -= deltaY;
            }

            startY += yDir;
            colRect.y += yDir;
            slope += deltaX;
            //checkBounds(screen_w, screen_h, false);
            sight = checkCollision(colRect.x, colRect.y, grid, false);
            if(!sight)
                break;
        }
        return sight;
    }
}



//This version of Bresenham's moves the player in as stright a line as possible to 
//the player
void Ooze::moveLine(std::vector<std::vector<Tile*>> &grid, SDL_Rect* target) {
    if(target == nullptr) return;
    int deltaX = target->x - rect.x;
    int deltaY = target->y - rect.y;
    int startX = rect.x;
    int startY = rect.y;
    int endX = target->x;
    int endY = target->y;
    int moveSlope = 0;
    int xDir = 0;
    int yDir = 0;

    deltaX = abs(deltaX * 2);
    deltaY = abs(deltaY * 2);

    if (target->y > rect.y) 
        yDir = 1;
	if (target->x > rect.x) 
        xDir = 1;
	if (target->y < rect.y) 
        yDir = -1;
	if (target->x < rect.x) 
        xDir = -1;

    if(deltaX > deltaY) {
        moveSlope = deltaY * 2 - deltaX;
        if(moveSlope >= 0) {
            startY += yDir;
            moveSlope -= deltaX;
            y_vel = yDir;
        }
        else
            y_vel = 0;

        startX += xDir;
        moveSlope += deltaY;
        x_vel = xDir;
    }
    else {
        moveSlope = deltaX * 2 - deltaY;
        if(moveSlope >= 0) {
            startX += xDir;
            moveSlope -= deltaY;
            x_vel = xDir;
        }
        else
            x_vel = 0;

        startY += yDir;
        moveSlope += deltaX;
        y_vel = yDir;
    }   
}

//If we don't see the player or a pickup, move to the next room
void Ooze::moveRoom(std::vector<std::vector<Tile*>> &grid) {
    SDL_Rect* temp1;
    SDL_Rect* temp2;
    SDL_Rect* intersect;
    std::vector<std::vector<Tile*>> map = tilemap->getMap();
    Tile* endTile;
    Tile* tile;
    Tile* doorTile;
    bool horWall = false;
    bool verWall = false; 
    int r = 0;
    int c = 0;
    int l = 0;
    int t = 0;
    roomTiles.startTile = nullptr;
    roomTiles.endTile = nullptr;
    roomTiles.door = nullptr;
    for(int i = 0; i < intersects.size(); i++) {
        intersect = &intersects[i];
       
        if(intersect->w > intersect->h && intersect->w > 2 && !horWall) {
            //horizontal wall
            r = intersect->y;
            c = intersect->x + (intersect->w/2);
            horWall = true;
            verWall = false;
        }
        else{
            r = intersect->y + (intersect->h/2);
            c = intersect->x;
            verWall = true;
            horWall = false;
        }
        doorTile = map[r][c];
        temp1 = doorTile->getDest();

        if(horWall) {
            t = c;
            if (temp1->y > rect.y) {
                l = r+2;
                r = r-1;
            }
            if (temp1->y < rect.y) {
                l = r-2;
                r = r+1;
            }
        }
        if(verWall) {
            l = r;
            if (temp1->x > rect.x) {
                t = c+2;
                c = c-1;
            }
            if (temp1->x < rect.x) {
                t = c-2;
                c = c+1;
            }
        }
        
        tile = map[r][c];
        temp1 = tile->getDest();
        std::cout << "RoomRect: X " << temp1->x << " Y " << temp1->y << " W " << temp1->w << " H " << temp1->h << std::endl;

        bool los = drawLine(grid, temp1);
        
        if(los && !doorTile->isVisited()) {
            roomTiles.door = temp1;
            lastRoom = tile;
            doorTile->setVisited(true);
            break;
        }            
    }    
    if(roomTiles.door == nullptr){
        tile = lastRoom;
        temp1 = doorTile->getDest();
        roomTiles.door = temp1;
        temp1 = tile->getDest();
        std::cout << "coo" << std::endl;
    }
    endTile = map[l][t];
    temp2 = endTile->getDest();
    roomTiles.startTile = temp1;
    roomTiles.endTile = temp2; 
    /* delete temp1;
    delete temp2;
    delete intersect;
    delete tile;
    delete endTile; */
/*
    SDL_Rect* temp1;
    SDL_Rect* temp2;
    SDL_Rect* intersect;
    std::vector<std::vector<Tile*>> map = tilemap->getMap();
    Tile* endTile;
    Tile* tile;
    Tile* doorTile;
    bool horWall = false;
    bool verWall = false; 
    int r = 0;
    int c = 0;
    int l = 0;
    int t = 0;
    roomTiles.startTile = nullptr;
    roomTiles.endTile = nullptr;
    roomTiles.door = nullptr;
    std::cout << intersects.size() << std::endl;
    for(int i = 0; i < doors.size(); i++) {
        
        temp1 = doors[i]->getDest();
        
        if(doors[i]->isHorWall()) {
            t = c;
            if (temp1->y > rect.y) {
                l = r+2;
                r = r-1;
            }
            if (temp1->y < rect.y) {
                l = r-2;
                r = r+1;
            }
        }
        if(!doors[i]->isHorWall()) {
            l = r;
            if (temp1->x > rect.x) {
                t = c+2;
                c = c-1;
            }
            if (temp1->x < rect.x) {
                t = c-2;
                c = c+1;
            }
        }    
        tile = map[r][c];
        temp1 = tile->getDest();
        std::cout << "RoomRect: X " << temp1->x << " Y " << temp1->y << " W " << temp1->w << " H " << temp1->h << std::endl;

        bool los = drawLine(grid, temp1);
        
        if(los && !doors[i]->isVisited()) {
            roomTiles.door = temp1;
            lastRoom = tile;
            doors[i]->setVisited(true);
            break;
        }            
    }    
    if(roomTiles.door == nullptr){
        tile = lastRoom;
        temp1 = doorTile->getDest();
        roomTiles.door = temp1;
        temp1 = tile->getDest();
        std::cout << "coo" << std::endl;
    }
    endTile = map[l][t];
    temp2 = endTile->getDest();
    roomTiles.startTile = temp1;
    roomTiles.endTile = temp2; 
    /*delete temp1;
    delete temp2;
    delete intersect;
    delete tile;
    delete endTile; */

}

//Lets make sure our poor ooze isn't stuck in a wall
void Ooze::initRoom(std::vector<std::vector<Tile*>> &grid) {
    int attempts = 0;
    while((collision::checkColLeft(rect, grid, 32) 
    || collision::checkColRight(rect, grid, 32) 
    ||collision::checkColTop(rect, grid, 32) 
    || collision::checkColBottom(rect, grid, 32)) && attempts < 30) {
        rect.x += tile_s;
        rect.y += tile_s;
    }
}

void Ooze::Mutate(){
    // Genetic statistics
    stats.health =      std::max(1, stats.health      + utils::normDist());
    stats.attack =      std::max(1, stats.attack      + utils::normDist()); //time delay between ticks damage
    stats.speed =       std::max(1, stats.speed       + utils::normDist());
    stats.health_cost = std::max(1, stats.health_cost + utils::normDist());
    stats.num_cost =    std::max(1, stats.num_cost    + utils::normDist());
    
    std::cout << "Ooze "  << oozeNumber  << ":"
    << " HP " << stats.health
    << " ATK " << stats.attack
    << " SPD " << stats.speed
    << " HC " << stats.health_cost
    << " NC " << stats.num_cost
    << "\n";
}


/* Summary
 * Argument
 *
 */
void Ooze::hurt(int damage) {
    stats.health -= damage;
    
    if ( stats.health <= 0 ) {
        state = DYING;
    }
}
    /* Summary
     * Argument
     *
     */
void Ooze::switchRoom() {
    if(collision::checkCol(roomRect, rect)) {
        return;
    }
    for(int i = 0; i < neighbors.size(); i++) {
        roomRect = neighbors[i]->getRectCopy();
        roomRect = {roomRect.x * TILE_SIZE, roomRect.y * TILE_SIZE, roomRect.w * TILE_SIZE, roomRect.h * TILE_SIZE};
        //std::cout << "RoomRect: X " << roomRect.x << " Y " << roomRect.y << " W " << roomRect.w << " H " << roomRect.h << std::endl;
        //std::cout << "Rect: X " << rect.x << " Y " << rect.y << " W " << rect.w << " H " << rect.h << std::endl;
        
        if(collision::checkCol(roomRect, rect)) {
            curRoom = neighbors[i];
            neighbors = curRoom->getNeighbors();
            intersects = curRoom->getIntersects();
            return;
        }
    }
}

void Ooze::changeColor(int r, int g, int b){
    
//    SDL_SetTextureColorMod(art->getImage(), art->getR(), art->getG(), art->getB());
//    SDL_RenderCopy(renderer, artifactList.at(inventory[x1][y1])->getImage(), NULL, &img);
    // art->getR(), art->getG(), art->getB());
    SDL_SetTextureColorMod(sheet.getTexture(), r, g, b);
    SDL_RenderCopy(renderer, sheet.getTexture(), NULL, getRect());
}
