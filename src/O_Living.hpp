#ifndef O_LIVING
#define O_LIVING
#include "Object.hpp"

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// LAYER 1
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

class LivingObject : public Object {
public:
    LivingObject(Object ** map, int x, int y, Texsheet* tex=nullptr, sf::Vector2i d = DOWN) : 
        Object(map, x, y, tex, d)
    {};

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
                return;
            }

            
            movementAnimation.frames = 6;
            movementAnimation.time = sf::milliseconds(10);
            movementAnimation.state = dtm[{dir.x, dir.y}] +4;
            movementAnimation.endPos = sf::Vector2i(pos.x * OBJECTUNIT, pos.y * OBJECTUNIT);

            sf::Vector2i deltaPos = pos - startpos;
            movementAnimation.move = sf::Vector2f( 
                ((float)deltaPos.x * (float)OBJECTUNIT) / (float)movementAnimation.frames, 
                ((float)deltaPos.y * (float)OBJECTUNIT) / (float)movementAnimation.frames);
        }
    };

    virtual bool update() override { 
        return true;
    }

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
public:

    Player(Object ** map, int x, int y) : 
        LivingObject(map, x, y, texsheets["panda"], DOWN)
    {
        switchTime = sf::milliseconds(600);
    };
    ~Player(){
        for (Object* i : bag) {
            free(i);
            std::cout << i << " from BAG deleted.\n";
        }
    };

    // TODO Hier und an der neighbor() scheitert aktuell die variable Objektgröße. Fix this!
    // z.B. fixbar durch loop, in der dann mit alllen adjazenten, interactable Objects interagiert wird.
    virtual bool interact(Object* interacter=nullptr) override{
        Object* nb = neighbor(dir);
        if(nb == nullptr) return false;
        return nb->getInteracted(this);
    };
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
    bool hasWon(){return won;}
    void win(){won=true;}
};

////////////////////////////////////////////////////////////////////////////////

class Enemy : public LivingObject {

};
////////////////////////////////////////////////////////////////////////////////



#endif //O_LIVING
