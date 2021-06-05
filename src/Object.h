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
    Object(Object**, int, int, Tex* = nullptr);
    Object** map;

    sf::Sprite sprite;
    int posX, posY, sizeX=1, sizeY=1; // Position ist immer oben links am Objekt. Size ist dessen Größe in Map-Nodes.
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
        Object(map, x, y, tex)
    {};

};

class Portal : public Object {
// Türen, Idee: statt aufschließen (permanent öffnen) kann man mit animation durchgehen, wenn man schlüssel hat. -> dann genau gleiche Funktion wie teleportationsportale.

};

class LivingObject : public Object {

};

class Player : public Object {
public:
    Player(Object ** map, int x, int y, Tex* tex) : 
        Object(map, x, y, tex)
    {};
    const int sizeX=1, sizeY=2;
};

class Enemy : public LivingObject {

};

class Item : public Object {

};

#endif //OBJECT_H
