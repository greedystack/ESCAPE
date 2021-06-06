#ifndef OBJECT_H
#define OBJECT_H

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include "Tex.h"

// Ein Objekt auf der Map. IDEE: Könnte von Sprite erben.
// Problem: Da draw() von Sprite private... 

class Object {
public:
    Object(Object**, int, int, int=1, int=1, Tex* = nullptr);
    Object** map;

    sf::Sprite sprite;
    int posX, posY, sizeX, sizeY; // Position ist immer oben links am Objekt. Size ist dessen Größe in Map-Nodes.
    bool visible=false;
    Tex *tex;

    void lookInDirection(char);


    bool move(int, int);
    bool stepRight();
    bool stepLeft();
    bool stepUp();
    bool stepDown();
    bool isFree(int, int, int, int);
    Object** getNode(int, int);
    Object* getObject(int, int);
    int getMapsizeX();
    int getMapsizeY();

};

// TODO Warum kann ich in der Vererbung keine Variablen überscheiben? Warum nimmt der immer die Vars der Mutterklasse?

class Barrier : public Object {
// Walls, Deko, ...
public:
    Barrier(Object ** map, int x, int y, Tex* tex) : 
        Object(map, x, y, 1, 1, tex)
    {};

};

class Portal : public Object {
// Türen, Idee: statt aufschließen (permanent öffnen) kann man mit animation durchgehen, wenn man schlüssel hat. -> dann genau gleiche Funktion wie teleportationsportale.

};

class LivingObject : public Object {
public:
    LivingObject(Object ** map, int x, int y, int sx, int sy, Tex* tex) : 
        Object(map, x, y, sx, sy, tex)
    {};
};

class Player : public LivingObject {
public:
    Player(Object ** map, int x, int y, Tex* tex) : 
        LivingObject(map, x, y, 2, 2, tex)
    {};

};

class Enemy : public LivingObject {

};



class Item : public Object {
public:
    Item (Object ** map, int x, int y, Tex* tex) : 
        Object(map, x, y, 1, 1, tex)
    {};
    bool collect(){
        // von Map entfernen
        // zu Bag hinzufügen
    };
    bool use(){
        // aus Bag entfernen
        // Animation
        // Wirkung (auf alle Objekte im Wirkradius)
    };
private:
    int distance; // Wie weit geht die Wirkung des Items, bis sie auf ein Objekt trifft?
    bool radial; // Falls true: Wirkung in alle Richtungen statt nur in Blickrichtung des Anwenders.
    virtual void effect(); // Die Wirkung. In Itemtypen zu implementieren!
};

#endif //OBJECT_H
