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
        Object(map, x, y, texsheets["wall"])
    {
        identity.insert(BARRIER);
    };

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
////////////////////////////////////////////////////////////////////////////////
// LAYER 2
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

class Goal : public Portal {
public:
    Goal(Object ** map, int x, int y) : 
        Portal(map, x, y, texsheets["wall"])
    {
        identity.insert(GOAL);
    };

    bool getInteracted(Object* interacter) override{
        std::cout << "GEWONNEN!" << std::endl;
        ((Player*)interacter)->win();
        return true;
    };
};


#endif //O_FACILITY