#ifndef O_LIVING
#define O_LIVING
#include "Object.hpp"

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// LAYER 1
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

class LivingObject : public Object {
protected:
    Texsheet* tex_moving;
    bool killed = false;
    // Animation:
    struct movementAnimation {
        uint state; // reihe in Texsheet
        uint frames = 0; // also animationsiterationen
        //uint blockCycles = 0; // block Update-cycles
        sf::Time time;
        sf::Vector2i endPos;
        sf::Vector2f move;
    } movementAnimation;
    struct specialAnimation {
        Texsheet* tex;
        uint frames=0;
        sf::Time time, last, passed;
    }specialAnimation;


////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////// 
public:
    LivingObject(Object ** map, int x, int y, Texsheet* tex=nullptr, sf::Vector2i d = DOWN) : 
        Object(map, x, y, tex, d)
    {
        identity.insert(LIVING);
    };
    bool wasKilled(){return killed;}
    // nur genau einen Schritt moven
    virtual void step(sf::Vector2i _dir, uint factor=1){
         // vorerst über teleport() -> Bei größeren objekten (also größer als 1x1) ist es aber sinnvoller, nicht alle Felder jedes Mal neu zu belegen, sondern immer nur das erste Feld in die zu bewegende richtung.
        if(dir != _dir){
            // erstmal nur in die gewünschte Richtung schauen. Noch kein Schritt.
            setDirection(_dir);
            return;
        }

        sf::Vector2i startpos = pos;
        for(uint i=1; i <= factor; i++){
            if(!setMapPosition(pos + _dir)){
                // Living Obj. ist wogegen gelaufen. -> interact.
                std::cout << "interaction throgh touching " << std::endl;
                interact(this);
                std::cout << "interaction throgh touching done" << std::endl;
                return;
            }

            
            movementAnimation.frames = 11*factor;
            movementAnimation.time = sf::milliseconds(9);
            movementAnimation.state = dtm[{dir.x, dir.y}];
            movementAnimation.endPos = sf::Vector2i(pos.x * OBJECTUNIT, pos.y * OBJECTUNIT);

            sf::Vector2i deltaPos = pos - startpos;
            movementAnimation.move = sf::Vector2f( 
                ((float)deltaPos.x * (float)OBJECTUNIT) / (float)movementAnimation.frames, 
                ((float)deltaPos.y * (float)OBJECTUNIT) / (float)movementAnimation.frames);
        }
    };

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
    public: virtual bool animate(sf::Time available) override{
        // FKT bekommt insgesamt noch für die ganze AnimationQueue zur Abarbeitung zur Verfügung stehende Zeit

        if(!animated)return false;

        if(movementAnimation.frames > 0){
            standingAnimation.passed = sf::milliseconds(0);
            return animateMovement(available); 
        }else if(specialAnimation.frames > 0){
            standingAnimation.passed = sf::milliseconds(0);
            return animateSpecial(available);
        }
        else return animateStanding(available);
    };

    bool animateMovement(sf::Time available){
        if(movementAnimation.frames <= 0) return false;
        sf::Time timePerStep = available / (float)movementAnimation.frames;
        if(timePerStep > movementAnimation.time){
            return false;
        }
        sprite.setTexture(tex_moving->texture);
        movementAnimation.frames--;

        sf::Vector2i texsize = (sf::Vector2i) tex_moving->getSize();
        uint curFrame = (movementAnimation.frames) % tex_moving->getImageCount().x;
        sf::Vector2i position(texsize.x * curFrame, texsize.y * movementAnimation.state);
        sprite.setTextureRect(sf::IntRect(position,texsize));

        

        std::cout << "Shifting to\tR: " << movementAnimation.state << "\tC: "<< curFrame << "\tRemaining: "<< movementAnimation.frames << "\tMove: "<< movementAnimation.move.x << ", " << movementAnimation.move.y << std::endl;

        sprite.move(movementAnimation.move);

        if(movementAnimation.frames <= 0){
                // setze endposition, falls floatingpointrechnungen unten böse numerik gemacht haben
                sprite.setPosition((sf::Vector2f)movementAnimation.endPos);
        }

        return true;

    }

    ///////////////////

    bool animateSpecial(sf::Time available){
        if(available > specialAnimation.last){
            specialAnimation.passed += specialAnimation.last + available - UPDATE_TIME;
        }else{
            specialAnimation.passed += specialAnimation.last - available;
        } 
        specialAnimation.last = available;

        if(specialAnimation.passed < specialAnimation.time){
            return false;
        }
        specialAnimation.passed = sf::milliseconds(0);

        specialAnimation.frames--;

        std::cout << "Remaining: "<< specialAnimation.frames << std::endl;

        if(specialAnimation.tex != nullptr){

            sprite.setTexture(specialAnimation.tex->texture);
            sf::Vector2i texsize = (sf::Vector2i) specialAnimation.tex->getSize();
            uint imgcount = specialAnimation.tex->getImageCount().x;

            uint curFrame = imgcount -((specialAnimation.frames) % imgcount) -1;
            sf::Vector2i position(texsize.x * curFrame, texsize.y * dtm[{dir.x, dir.y}]);
            sprite.setTextureRect(sf::IntRect(position,texsize));
        }

        return true;
    }

    ////////////////////////////////////////////////////////////////////////////////

};


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// LAYER 2
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
class Player : public LivingObject {
private:
    std::list<Object*> bag;
    bool won = false;
    uint food, navi, marker;
public:

    Player(Object ** map, int x, int y) : 
        LivingObject(map, x, y, texsheets["panda_standing"], DOWN)
    {
        identity.insert(PLAYER);
        tex_moving = texsheets["panda_move"];
        food = 10;
    };
    ~Player(){
        for (Object* i : bag) {
            free(i);
            std::cout << i << " from BAG deleted.\n";
        }
    };

    virtual void step(sf::Vector2i _dir, uint factor=1) override {
        LivingObject::step(_dir, factor);
        for(auto d : {RIGHT, UP, LEFT, DOWN}){
            if(neighbor(d) == nullptr) continue;
            if(neighbor(d)->whoami().contains(ENEMY)){
                setDirection(d);
                interact(this);
            }
        }
    }

    // Hier und an der neighbor() scheitert aktuell die variable Objektgröße. Fix this!
    // z.B. fixbar durch loop, in der dann mit alllen adjazenten, interactable Objects interagiert wird.
    virtual bool interact(Object* interactee=nullptr) override {
        if(interactee == nullptr || interactee == this){
            interactee = neighbor(dir);
        }
        if(interactee == nullptr) return false;

        if(interactee->whoami().contains(ENEMY)){
            if(!((LivingObject*)interactee)->wasKilled()){
                if(food >= 5){
                    specialAnimation.tex=texsheets["panda_kill"];
                    specialAnimation.frames=11;
                    specialAnimation.time = sf::milliseconds(30);
                    interactee->getInteracted(this);
                    food -= 5;
                }else{
                    specialAnimation.tex=texsheets["panda_killed"];
                    specialAnimation.frames=19;
                    specialAnimation.time = sf::milliseconds(50);
                    
                    interactee->interact(this);
                    killed = true;
                }
            }
            return true;
        }
        else if(interactee->whoami().contains(GOAL)){
            win();
            return interactee->getInteracted(this);
        }
        else if(interactee->whoami().contains(FOOD)){
            if(food < 5) food++;
            interactee->getInteracted(this);
        }
        else if(interactee->whoami().contains(NAVI)){
            navi++;
            interactee->getInteracted(this);
        }
        else if(interactee->whoami().contains(MARKER)){
            marker++;
            interactee->getInteracted(this);
        } 
        else {
            return interactee->getInteracted(this);
        }

        return false;
    };

    virtual bool getInteracted(Object* interacter=nullptr) override {
        if(interacter->whoami().contains(ENEMY)){
            setDirection(invertDirection(interacter->dir));
            interact(this);
        }
    }


    bool hasWon(){return won;}
    bool isParalyzed(){return specialAnimation.frames > 0;}

    void win(){
        movementAnimation.frames = 3;
        movementAnimation.time = sf::milliseconds(100);
        movementAnimation.state = dtm[{dir.x, dir.y}];

        sf::Vector2i endPos = pos + dir;
        movementAnimation.endPos = sf::Vector2i(endPos.x * OBJECTUNIT, endPos.y * OBJECTUNIT);

        sf::Vector2i deltaPos = endPos - pos;
        movementAnimation.move = sf::Vector2f( 
            ((float)deltaPos.x * (float)OBJECTUNIT) / (float)movementAnimation.frames, 
            ((float)deltaPos.y * (float)OBJECTUNIT) / (float)movementAnimation.frames);

        // Lähmen:
        specialAnimation.tex=nullptr;
        specialAnimation.frames = 3;
        specialAnimation.time = sf::milliseconds(50);
        won=true;
    }



    void putInBag(Object* item){
        bag.push_back(item);
        std::cout << "added " << item << "\n\n";
        for (Object* i : bag) {
            std::cout << i << std::endl;
        }
    };
    void removeFromBag(Object* item){
        bag.remove(item);
        std::cout << "removed " << item << "\n\n";
        for (Object* i : bag) {
            std::cout << i << std::endl;
        }
    }
    void useItem(){
        if(bag.empty()) return;
        if(bag.front()->interact(this)) removeFromBag(bag.front());
    }
};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

class Enemy : public LivingObject {
public:
    Enemy(Object ** map, int x, int y) : 
        LivingObject(map, x, y, texsheets["panda_standing"], UP)
    {
        identity.insert(ENEMY);
        tex_moving = texsheets["panda_move"];
    };

    virtual bool interact(Object* interactee=nullptr) override{
        if(interactee->whoami().contains(PLAYER)){
            // Töte Player
            sf::Vector2i playerDir = interactee->pos - pos;
            setDirection(playerDir);
            
            specialAnimation.tex=texsheets["panda_kill"];
            specialAnimation.frames=11;
            specialAnimation.time = sf::milliseconds(70);
        }
    };

    virtual bool getInteracted(Object* interacter=nullptr) override{
        if(interacter->whoami().contains(PLAYER)){
            // Wurde vom Player getötet.
            sf::Vector2i playerDir = interacter->pos - pos;
            setDirection(playerDir);

            specialAnimation.tex=texsheets["panda_killed"];
            specialAnimation.frames=19;
            specialAnimation.time = sf::milliseconds(80);
            killed = true;
        }
    };

    virtual void step(sf::Vector2i _dir, uint factor=1) override {
        LivingObject::step(_dir, factor);
        for(auto d : {RIGHT, UP, LEFT, DOWN}){
            if(neighbor(d) == nullptr) continue;
            if(neighbor(d)->whoami().contains(PLAYER)){
                setDirection(d);
                neighbor(d)->getInteracted(this);
            }
        }
    }

    virtual void update() override{
        if(specialAnimation.frames < 1){
            if(killed){
                del();
            }
            else{
                // Do a step ... maybe.
                uint r = rand() % 5;
                if(r == 0){
                    sf::Vector2i _dir;
                    r = rand() % 2;
                    if(r == 0){
                        // choose direction
                        r = rand() % 4;
                        std::array directions = {LEFT, UP, RIGHT, DOWN};
                        _dir = directions[r];
                    } else {
                        // use same direction
                        _dir = dir;
                    }
                    step(_dir);
                }
            }
        }
    }

    
    
    //virtual void kill() override {del();}

};
////////////////////////////////////////////////////////////////////////////////



#endif //O_LIVING
