#include "include/ooze.h"

//SDL_Rect rect;
//SpriteSheet sheet;

//initialize static member variables
int Ooze::totalOoze = 0;


//int x_vel;
//int y_vel;

constexpr int MAX_SPEED = 1;
constexpr int BORDER_SIZE = 32;

// Default Constructor
Ooze::Ooze():state{ROAMING}, hostility{0} {}

//Constructor from rect
<<<<<<< HEAD
Ooze::Ooze(SDL_Rect _rect, Player *player, HUD *h):player{player},state{roaming}, hostility{0} {
    rect = _rect;
    this->player = player;
	  hud = h;
	  totalOoze++; //Increase # of instances counter
	  oozeNumber = totalOoze;
	  Animation* anim;
	  int overlapTicks = 0;
    //Speed
    x_deltav = 0;
    y_deltav = 0;
    x_vel = 1;
    y_vel = 1;
=======
Ooze::Ooze(SDL_Rect _rect, Player *player, HUD *h):state{ROAMING}, hostility{0} {
    rect = _rect;
    oozePlayer = player;
    target = player->getRect();
	hud = h;
	totalOoze++; //Increase # of instances counter
	oozeNumber = totalOoze;
	Animation* anim;
	int overlapTicks = 0;
    //Speed
    o_x_deltav = 0;
    o_y_deltav = 0;
    o_x_vel = 0;
    o_y_vel = 0;

    ate = 0;
>>>>>>> 8d7246e77bbbcd56866d099d56131757c1cb856a
}

//Other constructor?
//Ooze::Ooze(State st, int hostil) :state{st}, hostility{hostil} {}

//Destructor
Ooze::~Ooze(){};

std::string Ooze::getInstanceName(){
	return "Ooze-"+ std::to_string(oozeNumber);
}

void Ooze::input(const Uint8* keystate){}

void Ooze::init(SDL_Renderer* gRenderer) {
	setSpriteSheet(utils::loadTexture(gRenderer, "res/ooze.png"), 3, 1);
    addAnimation("wandering", Animation(getSheet().getRow(0)));
    setAnimation("wandering");
	
    

	//This should be removed ASAP
    lWall = {screen_w/4, screen_h/4, screen_w/12, screen_h/2};
	rWall = {screen_w/4 * 3 - screen_w/12, screen_h/4, screen_w/12, screen_h/2};
	uWall = {screen_w/4, screen_h/4, screen_w/2, screen_h/12};
	cPillar = {screen_w/2, screen_h/2 + (tile_s * 5), tile_s};
}

void Ooze::setSpriteSheet(SDL_Texture* _sheet, int _cols, int _rows) {
    sheet = SpriteSheet(_sheet);
    sheet.setClips(_cols, _rows, rect.w, rect.h);
}

//*********TO DO:
//update motion here
void Ooze::update(std::unordered_map<std::string, Object*> *objectList, Uint32 ticks) {
	
	int x_deltav = 0;
	int y_deltav = 0;
    
    //Get the position of the player before they move
    //Needed for collision detection
    int curX = rect.x;
    int curY = rect.y;

    int pickupX;
    int pickupY;
	
    //might move order of update calls
    bool stateChange = updateState(objectList, ticks);

	bool overlap = checkOozeOverlap(objectList, ticks);
    
    //target = getPickup(objectList)->getRect();
	if(!overlap){

        //uncomment the line below to change the ooze to chasing the pickups
        //target = pickTarget(objectList);

		//check which direction the player is
<<<<<<< HEAD
		if (player->getY() > rect.y + rect.h)
			y_deltav += 1;
		if (player->getX() > rect.x + rect.w)
			x_deltav += 1;
		if (player->getY() + player->getHeight() < rect.y)
			y_deltav -= 1;
		if (player->getX() + player->getWidth() < rect.x)
			x_deltav -= 1;
        
        updateVelocity(x_deltav,y_deltav);
=======
        if (target->y > rect.y)
            o_y_deltav += 1;
        if (target->x > rect.x)
            o_x_deltav += 1;
        if (target->y < rect.y)
            o_y_deltav -= 1;
        if (target->x < rect.x)
            o_x_deltav -= 1;
        /*
		if (oozePlayer->getY() > rect.y)
			o_y_deltav += 1;
		if (oozePlayer->getX() > rect.x)
			o_x_deltav += 1;
		if (oozePlayer->getY() < rect.y)
			o_y_deltav -= 1;
		if (oozePlayer->getX() < rect.x)
			o_x_deltav -= 1;*/

		updateVelocity(o_x_deltav, o_y_deltav);

		// Move box
		updatePosition();

		// Check you haven't moved off the screen
		checkBounds(screen_w, screen_h);

		//Check you haven't collided with object
		checkCollision(curX, curY);
>>>>>>> 8d7246e77bbbcd56866d099d56131757c1cb856a
	}
    //foundFood(getPickup(objectList));
    //update animation
    updateAnimation(ticks);
<<<<<<< HEAD

    updatePosition();
    checkBounds(screen_w, screen_h);
    //Check you haven't collided with object
    checkCollision(curX, curY);
=======
}

// Might not need this function. Original purpose was to parse through object list and
// find a pickup. Instead, search for an ooze from the pickup.cpp's perspective.
Pickup* Ooze::getPickup(std::unordered_map<std::string, Object*> *objectList) {
    std::unordered_map<std::string, Object*>::iterator it = objectList->begin();
    while(it != objectList->end()){
        if (!it->first.substr(0,6).compare("Pickup")) {
            //std::cout << "there is a pickup :) " << std::endl;
            return (Pickup*)it->second;
        }
        it++;
    }
    return NULL;
}

SDL_Rect* Ooze::pickTarget(std::unordered_map<std::string, Object*> *objectList) {
    std::unordered_map<std::string, Object*>::iterator it = objectList->begin();
    while(it != objectList->end()){
        if (!it->first.substr(0,6).compare("Pickup")) {
            //std::cout << "there is a pickup :) " << std::endl;
            Pickup* temp = (Pickup*)it->second;
            return temp->getRect();
        }
        it++;
    }
    return oozePlayer->getRect();
}

// Checks for overlap between ooze and a given pickup
// TODO: combine this with the overlap method below, which
//       checks for overlap betw. ooze and player
bool Ooze::foundFood(Pickup* food) {
    if (food) {
        SDL_Rect* fRect = food->getRect();
        bool overlap = collision::checkCol(rect, *fRect);
        if (overlap) {
            //food->use();
            ate = ate + 1;
            std::string s = getInstanceName() + " ATE: "+ food->getInstanceName() + ". HAS ATE: " + std::to_string(ate);
            std::cout << s << std::endl;
            return true;
        }
    }
    return false;
}

int Ooze::getAte() {
    return ate;
}

bool Ooze::updateState(std::unordered_map<std::string, Object*> *objectList, Uint32 ticks) {
    return false;
>>>>>>> 8d7246e77bbbcd56866d099d56131757c1cb856a
}

void Ooze::increaseHostility() {
	if (hostility < 10)
		hostility++;
}
void Ooze::decreaseHostility() {
	if (hostility >  0)
		hostility--;
}

SDL_Renderer* Ooze::draw(SDL_Renderer* renderer) {
    SDL_RenderCopy(renderer, sheet.getTexture(), anim->getFrame(), getRect());
   return renderer;
}

//Checks if the player overlapped with the ooze and acts accordingly
//based on pickup's method
bool Ooze::checkOozeOverlap(std::unordered_map<std::string, Object*> *objectList, Uint32 ticks) {
	SDL_Rect* pRect = player->getRect();
	bool overlap = collision::checkCol(rect, *pRect);

	if (overlap) {
		overlapTicks += ticks;
<<<<<<< HEAD
		if (overlapTicks > 25) {
=======
		if (overlapTicks > 1000) {
>>>>>>> 8d7246e77bbbcd56866d099d56131757c1cb856a
			hud->currentHealth = std::max(0, hud->currentHealth-damage);
			std::string s = "HIT: "+getInstanceName();
			std::cout << s << std::endl;
			overlapTicks = 0;
		}
	} else {
		overlapTicks = 0;
	}

	player->setEnemy(overlap);

	return overlap;
}

void Ooze::updateAnimation(Uint32 ticks) {

    if(true) { //ticks/10%2 == 2
        setAnimation("wandering");
        anim->play();
        //std::cout << "ooze animating";
    }
    else {
        anim->reset();
        anim->stop();
    }
    anim->update(ticks);
}

<<<<<<< HEAD
void Ooze::updatePosition() {
    rect.x += x_vel;
    rect.y += y_vel;
}

void Ooze::checkBounds(int max_width, int max_height) {
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

=======
>>>>>>> 8d7246e77bbbcd56866d099d56131757c1cb856a
bool Ooze::isUsed() { return false; }

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
    /*
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
     */
    
    // Speed up/slow down
    x_vel += _xdv;
    y_vel += _ydv;

    // Check speed limits
<<<<<<< HEAD
    if (x_vel < -1 * MAX_SPEED)
        x_vel = -1 * MAX_SPEED;
    else if (x_vel > MAX_SPEED)
        x_vel = MAX_SPEED;

    if (y_vel < -1 * MAX_SPEED)
        y_vel = -1 * MAX_SPEED;
    else if (y_vel > MAX_SPEED)
        y_vel = MAX_SPEED;
=======
    if (o_x_vel < -1 * MAX_SPEED)
        o_x_vel = -1 * MAX_SPEED;
    else if (o_x_vel > MAX_SPEED)
        o_x_vel = MAX_SPEED;

    if (o_y_vel < -1 * MAX_SPEED)
        o_y_vel = -1 * MAX_SPEED;
    else if (o_y_vel > MAX_SPEED)
        o_y_vel = MAX_SPEED;

    // Also update position
   this->updatePosition();
}

void Ooze::updatePosition() {
    rect.x += o_x_vel;
    rect.y += o_y_vel;
}

//checks bounds of screen
void Ooze::checkBounds(int max_width, int max_height) {
    if (rect.x < BORDER_SIZE)
        rect.x = BORDER_SIZE;
    else if (rect.x + rect.w > max_width - BORDER_SIZE)
        rect.x = max_width - rect.w - BORDER_SIZE;

    if (rect.y < BORDER_SIZE)
        rect.y = BORDER_SIZE;
    else if (rect.y + rect.h > max_height - BORDER_SIZE)
        rect.y = max_height - rect.h - BORDER_SIZE;
>>>>>>> 8d7246e77bbbcd56866d099d56131757c1cb856a
}

//currently checks collisions with room features (walls etc.)
void Ooze::checkCollision(int curX, int curY)
{
    //Checks the collision of each object and determines where the player should stop
    //In the future, we might need to alter this function to take in an object that
    //represents what the player is colliding with. This shouldn't be too difficult

    //LEFT WALL
    if(collision::checkCol(rect, lWall))
    {
        rect.x = curX;
    }
    if(collision::checkCol(rect, lWall))
    {
        rect.y = curY;
		//If this is not included the x movement will lock when colliding with y
		rect.x += x_vel;
    }

    //RIGHT WALL
    if(collision::checkCol(rect, rWall))
    {
        rect.x = curX;
    }
    if(collision::checkCol(rect, rWall))
    {
        rect.y = curY;
		rect.x += x_vel;
    }

    //UPPER WALL
    if(collision::checkCol(rect, uWall))
    {
        rect.x = curX;
    }
    if(collision::checkCol(rect, uWall))
    {
        rect.y = curY;
		rect.x += x_vel;
    }

    //PILLAR - very difficult to implement with this style
    if(collision::checkCol(rect, cPillar))
    {
        rect.x = curX;
    }
    if(collision::checkCol(rect, cPillar))
    {
        rect.y = curY;
		rect.x += x_vel;
    } 
}
