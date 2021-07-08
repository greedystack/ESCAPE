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
    std::map<uint, Texsheet*> tex_special;
    // Animation:
    struct movementAnimation {
        uint state; // reihe in Texsheet
        uint frames = 0; // also animationsiterationen
        //uint blockCycles = 0; // block Update-cycles
        sf::Time time;
        sf::Vector2i endPos;
        sf::Vector2f move;
        bool interact;
    } movementAnimation;

    struct specialAnimation {
        uint type;
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
                std::cout << "interaction throgh touching" << std::endl;
                interact(this);
                return;
            }

            
            movementAnimation.frames = 6*factor;
            movementAnimation.time = sf::milliseconds(10);
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


                // TODO das dann doch lieber über eine queue lösen... Hier müsste eigentlich dann direkt die kill/getkilled animation folgen.
                if(movementAnimation.interact){
                    movementAnimation.interact = false;
                    interact(this);
                }
        }

        return true;

    }

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

        if(tex_special[specialAnimation.type] != nullptr){

            sprite.setTexture(tex_special[specialAnimation.type]->texture);
            sf::Vector2i texsize = (sf::Vector2i) tex_special[specialAnimation.type]->getSize();
            uint curFrame = (specialAnimation.frames) % tex_special[specialAnimation.type]->getImageCount().x;
            sf::Vector2i position(texsize.x * curFrame, texsize.y * dtm[{dir.x, dir.y}]);
            sprite.setTextureRect(sf::IntRect(position,texsize));
        }


        if(specialAnimation.frames <= 0){
            switch(specialAnimation.type){
                case KILL:
                    
                    break;
                case KILLED:
                    kill();
                    break;
                default:
                    break;
            }
        }



        return true;
    }

    virtual void kill(){}

};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// LAYER 2
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
class Player : public LivingObject {
private:
    std::list<Object*> bag;
    bool won = false, killed = false;
    uint bambus, navi, bread;
public:

    Player(Object ** map, int x, int y) : 
        LivingObject(map, x, y, texsheets["panda_standing"], DOWN)
    {
        identity.insert(PLAYER);
        tex_special[KILL] = texsheets["panda_killed"];
        tex_special[KILLED] = texsheets["panda_killed"];
        tex_moving = texsheets["panda_move"];
        bambus = 5;
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
    virtual bool interact(Object* interacter=nullptr) override{
        Object* nb = neighbor(dir);
        if(nb == nullptr) return false;
        if(nb->whoami().contains(ENEMY)){
            if(bambus >= 5){
                specialAnimation.type=KILL;
                specialAnimation.frames=19;
                specialAnimation.time = sf::milliseconds(100);
                
                bambus -= 5;
                return true;
            }else{
                specialAnimation.type=KILLED;
                specialAnimation.frames=19;
                specialAnimation.time = sf::milliseconds(100);

                //killed = true;
                return true;
            }
            return true;
        }
        return nb->getInteracted(this);
    };


    bool hasWon(){return won;}
    virtual void kill() override {killed = true;}
    bool wasKilled(){return killed;}
    bool isParalyzed(){return specialAnimation.frames > 0;}

    void win(){
        movementAnimation.frames = 6;
        movementAnimation.time = sf::milliseconds(10);
        movementAnimation.state = dtm[{dir.x, dir.y}] +4;

        sf::Vector2i endPos = pos + dir;
        movementAnimation.endPos = sf::Vector2i(endPos.x * OBJECTUNIT, endPos.y * OBJECTUNIT);

        sf::Vector2i deltaPos = endPos - pos;
        movementAnimation.move = sf::Vector2f( 
            ((float)deltaPos.x * (float)OBJECTUNIT) / (float)movementAnimation.frames, 
            ((float)deltaPos.y * (float)OBJECTUNIT) / (float)movementAnimation.frames);
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
        LivingObject(map, x, y, texsheets["panda"], UP)
    {
        identity.insert(ENEMY);
    };

    virtual bool interact(Object* interactee=nullptr) override{
        if(interactee->whoami().contains(PLAYER)){
            // Töte Player
            sf::Vector2i playerDir = interactee->pos - pos;
            setDirection(playerDir);
            
            // TODO Killanimation
        }
    };

    virtual bool getInteracted(Object* interacter=nullptr) override{
        if(interacter->whoami().contains(PLAYER)){
            // Wurde vom Player getötet.
            // TODO Sterbeanimation
            cout << "enemy killed" << endl;
            del();
        }
    };

    virtual void kill() override {del();}

};
////////////////////////////////////////////////////////////////////////////////



#endif //O_LIVING
