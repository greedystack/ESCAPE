#ifndef O_FACILITY
#define O_FACILITY
#include "Object.hpp"

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// LAYER 1
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

class Barrier : public Object {
// Walls, Deko, ...
public:
    Barrier(Object ** map, int x, int y) : 
        Object(map, x, y, texsheets["hecke"])
    {
        identity.insert(BARRIER);

        
        for(auto d : {RIGHT, UP, LEFT, DOWN}){
            if(neighbor(d) == nullptr) continue;            
            if(neighbor(d)->whoami().contains(BARRIER)){
                neighbor(d)->updateDirection();
            }
            
        }
        updateDirection();
        
    };

    virtual void updateDirection() override {
        // hier wird die Row des Texsheets direkt in den x-Value der Direction kodiert, da die von den Nachbarn und deren Position abhängt.
        std::set<std::array<int, 2>> bnd; //barrierNeighborDirections ... oder Bundesnachrichtendienst?? ... Zufall oder Chiffre?!!
        for(auto d : {RIGHT, UP, LEFT, DOWN}){
            if(neighbor(d) == nullptr) continue;
            if(neighbor(d)->whoami().contains(BARRIER)){
                bnd.insert({d.x, d.y});
            }
        }

        std::array<int, 2> R, L, U, D;
        R= {1, 0}; L= {-1, 0}; U= {0, -1}; D= {0, 1};

        std::map< std::set<std::array<int, 2>>, int> mapping;
        mapping[{R, D}] = 0;
        mapping[{L, D}] = 1;
        mapping[{U, R}] = 2;
        mapping[{U, L}] = 3;
        mapping[{U, D, R}] = 4;
        mapping[{U, D, L}] = 5;
        mapping[{R, L, D}] = 6;
        mapping[{R, L, U}] = 7;
        mapping[{R, L}] = 8;
        mapping[{U, D}] = 9;
        mapping[{L}] = 10;
        mapping[{R}] = 11;
        mapping[{D}] = 12;
        mapping[{U}] = 13;
        mapping[{}] = 15;
        mapping[{R, L, U, D}] = 14;

        int spriterow = mapping[bnd];
        dir = sf::Vector2i(spriterow, 0);
        setTexType(spriterow);
        
    }

};

////////////////////////////////////////////////////////////////////////////////

class Portal : public Object {
// Türen, Idee: statt aufschließen (permanent öffnen) kann man mit animation durchgehen, wenn man schlüssel hat. -> dann genau gleiche Funktion wie teleportationsportale.
public:
    Portal(Object ** map, int x, int y, Texsheet* t) : 
        Object(map, x, y, t)
    {
        identity.insert(PORTAL);
    };

};


////////////////////////////////////////////////////////////////////////////////


class Collectable : public Object {
public:
    Collectable(Object ** map, int x, int y, Texsheet* t) : 
        Object(map, x, y, t)
    {
        identity.insert(COLLECTABLE);
    };

    virtual bool getInteracted(Object* interacter=nullptr) override{
        del();
        return true;
    }
};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// LAYER 2
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

class Goal : public Portal {
public:
    Goal(Object ** map, int x, int y) : 
        Portal(map, x, y, texsheets["portal"])
    {
        identity.insert(GOAL);
        setTexType(0);
        standingAnimation.time=sf::milliseconds(75);
        
    };

};

class Start : public Portal {
public:
    Start(Object ** map, int x, int y) : 
        Portal(map, x, y, texsheets["portal"])
    {
        identity.insert(START);
        setTexType(1);
    };

};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

class Food : public Collectable {
public:
    Food(Object** map, int x, int y) : 
        Collectable(map, x, y, texsheets["bambus"])
    {
        identity.insert(FOOD);
        setTexType(0);
        
    };
};

class Navi : public Collectable {
public:
    Navi(Object** map, int x, int y) : 
        Collectable(map, x, y, texsheets["navi"])
    {
        identity.insert(NAVI);
        setTexType(0);
        
    };
};


class Marker : public Collectable {
public:
    Marker(Object** map, int x, int y) : 
        Collectable(map, x, y, texsheets["bread"])
    {
        identity.insert(MARKER);
        setTexType(0);
        
    };
};


class Arrow : public Object {
public:
    Arrow() : 
        Object(nullptr, 0, 0, texsheets["arrow"])
    {
        identity.insert(ARROW);
        setTexType(0);
        animated=true;
        standingAnimation.time = sf::milliseconds(175);
        
    };
};


#endif //O_FACILITY
