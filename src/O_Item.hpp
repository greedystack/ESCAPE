#ifndef O_ITEM
#define O_ITEM
#include "Object.hpp"

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// LAYER 1
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

class Item : public Object {
public:
    Item (Object ** map, int x, int y, int _distance=1, bool _rad=false) : 
        Object(map, x, y, texsheets["wall"]), distance(_distance), radial(_rad)
    {
        identity.insert(ITEM);
    };
    bool collect(Player* collector){
        removeFromMap();
        collector->putInBag(this);
        return true;
    };
    Object* getInteractee(sf::Vector2i _pos, sf::Vector2i _dir){
        // gibt nächstes Interaktionsziel (gemäß distance) zurück. 
        // TODO bei Implementierung von radial müssen mehrere Ziele zurückgegeben und verarbeitet werden.
        for(uint i=1; i <= distance; i++){
            _pos+=_dir;
            if(getObject(_pos) != nullptr) return getObject(_pos);
        }
        return nullptr;
    }

    // standardinteraktion: Item wieder ablegen.
    virtual bool interact(Object* player) override{
        std::cout << "placing item" << this << std::endl;
        sf::Vector2i newpos = player->pos + player->dir;
        if(getObject(newpos) != nullptr) return false;
        pos = newpos;
        sprite.setPosition((float)pos.x*OBJECTUNIT, (float)pos.y*OBJECTUNIT);
        place();
        return true;
    };
    // Item aufsammeln
    bool getInteracted(Object* interacter) override{
        std::cout << "Huch, mit mir wurde interagiert." << std::endl;
        return collect((Player*)interacter);
    };

private:
    int distance; // Wie weit geht die Wirkung des Items, bis sie auf ein Objekt trifft?
    bool radial; // Falls true: Wirkung in alle Richtungen statt nur in Blickrichtung des Anwenders.
    //virtual void effect(); // Die Wirkung. In Itemtypen zu implementieren!
};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// LAYER 2
////////////////////////////////////////////////////////////////////////////////
///// Alles, was Objekte zerstört (Enemies tötet, Wände einreißt, ...)
////////////////////////////////////////////////////////////////////////////////
class Destroyer : public Item {
public:
    Destroyer (Object ** map, int x, int y, int _distance=5, bool _rad=false) :
        Item (map, x, y, _distance, _rad)
    {
        identity.insert(DESTROYER);
    };
    

    virtual bool interact(Object* player) override{
        Object* target = getInteractee(player->pos, player->dir);
        if(target->whoami().contains(BARRIER)){
            std::cout << "FEUER FREI! \n";
            target->del();
            return true;
        }
        return false;
    };

};

////////////////////////////////////////////////////////////////////////////////

#endif //O_ITEM
