#ifndef O_PLAYER
#define O_PLAYER
#include "O_Living.hpp"

const uint FOOD_NEEDED_TO_KILL = 2;

class Player : public LivingObject {
private:
    std::list<Object*> bag;
    bool won = false;
    Object navi = Arrow();
    std::set<std::array<uint, 2>> markedFields;
public:
    Itempanel itempanel;

    Player(Object ** map, int x, int y) : 
        LivingObject(map, x, y, texsheets["panda_standing"], DOWN), itempanel(0)
    {
        //itempanel = Itempanel(0);

        identity.insert(PLAYER);
        tex_moving = texsheets["panda_move"];

        itempanel.add(Itempanel::NAVI, 3);
        itempanel.add(Itempanel::MARKER, 3);
        itempanel.add(Itempanel::FOOD, 4);


        ///////////////////////////////
        
    };
    ~Player(){
        for (Object* i : bag) {
            free(i);
            std::cout << i << " from BAG deleted.\n";
        }
    };

    virtual uint step(sf::Vector2i _dir, uint factor=1) override {
        if(getMarkerSteps() > 0) markedFields.insert({(uint)pos.x, (uint)pos.y});

        uint steps = LivingObject::step(_dir, factor);
        for(auto d : {RIGHT, UP, LEFT, DOWN}){
            if(neighbor(d) == nullptr) continue;
            if(neighbor(d)->whoami().contains(ENEMY)){
                //setDirection(d);
                interact(neighbor(d));
            }
        }

        itempanel.step(steps);
        return steps;
    }

    // Hier und an der neighbor() scheitert aktuell die variable Objektgröße. Fix this!
    // z.B. fixbar durch loop, in der dann mit alllen adjazenten, interactable Objects interagiert wird.
    virtual bool interact(Object* interactee=nullptr) override {
        if(interactee == nullptr || interactee == this){
            interactee = neighbor(dir);
        }
        if(interactee == nullptr) return false;

        // TODO: Animation queue! 
        if(interactee->whoami().contains(ENEMY)){
            if(!((LivingObject*)interactee)->wasKilled()){
                sf::Vector2i _dir = interactee->pos - pos;
                if(itempanel.use(Itempanel::FOOD, FOOD_NEEDED_TO_KILL)){
                    interactee->getInteracted(this);
                    enqueueSpecialAnimation(texsheets["panda_kill"], 11, 20, _dir);
                    enqueueSpecialAnimation(texsheets["panda_kill"], 11, 15, _dir, true);
                    
                }else{
                    interactee->interact(this);
                    enqueueSpecialAnimation(texsheets["panda_killed"], 19, 70, _dir);
                    
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
            if(itempanel.add(Itempanel::FOOD))
                interactee->getInteracted(this);
        }
        else if(interactee->whoami().contains(NAVI)){
            if(itempanel.add(Itempanel::NAVI))
                interactee->getInteracted(this);
        }
        else if(interactee->whoami().contains(MARKER)){
            if(itempanel.add(Itempanel::MARKER))
                interactee->getInteracted(this);
        } 
        else {
            return interactee->getInteracted(this);
        }

        return false;
    };

    virtual bool getInteracted(Object* interacter=nullptr) override {
        if(interacter->whoami().contains(ENEMY)){
            //setDirection(invertDirection(interacter->dir));
            interact(interacter);
        }
    }

    
    bool hasWon(){return won;}
    bool isParalyzed(){return saq.size() > 0;}

    void win(){
        movementAnimation.frames = 5;
        movementAnimation.time = sf::milliseconds(16);
        movementAnimation.state = dtm[{dir.x, dir.y}];

        sf::Vector2i endPos = pos + dir;
        movementAnimation.endPos = sf::Vector2i(endPos.x * OBJECTUNIT, endPos.y * OBJECTUNIT);

        sf::Vector2i deltaPos = endPos - pos;
        movementAnimation.move = sf::Vector2f( 
            ((float)deltaPos.x * (float)OBJECTUNIT) / (float)movementAnimation.frames, 
            ((float)deltaPos.y * (float)OBJECTUNIT) / (float)movementAnimation.frames);

        
        enqueueSpecialAnimation(texsheets["panda_on_portal"], 18, 50);
        won=true;
    }

    //////////////////////////////

    bool animateNavi(sf::Time time){
        if(won) return false;
        if(itempanel.getSteps(Itempanel::NAVI) > 0){
            return navi.animate(time);
        }
        return false;
    }

    Object* getNavi(sf::Vector2i _dir){
        if(won) return nullptr;
        if(itempanel.getSteps(Itempanel::NAVI) > 0){
            navi.teleport(pos+_dir);
            navi.setDirection(_dir);
            return &navi;
        }
        return nullptr;
    }


    int getMarkerSteps(){return itempanel.getSteps(Itempanel::MARKER);}
    bool isMarked(uint x, uint y){return markedFields.contains({x, y});}

    int getFood(){return itempanel.getSteps(Itempanel::FOOD);}

    //////////////////////////////
    
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

#endif