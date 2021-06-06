#ifndef OBJECT_H
#define OBJECT_H

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include "Tex.h"
#include<iostream>

// Ein Objekt auf der Map. IDEE: Könnte von Sprite erben.
// Problem: Da draw() von Sprite private... 

const sf::Vector2<int> RIGHT(1,0);
const sf::Vector2<int> LEFT(-1,0);
const sf::Vector2<int> UP(0,-1);
const sf::Vector2<int> DOWN(0,1);

class Object {
public:
    Object** map;
    sf::Sprite sprite;
    sf::Vector2<int> pos, size; // Position ist immer oben links am Objekt. Size ist dessen Größe in Map-Nodes.

    bool visible=false;
    Tex *tex;
    ////////////////////////////////////////////////////////////////////////////////

    Object(Object** m, int x, int y, int sx=1, int sy=1, Tex* t = nullptr) 
        : map(m), pos(x, y), size(sx, sy), tex(t)
    {
        if (tex != nullptr){
            visible=true;
            sprite.setTexture(tex->regular);
            sprite.setOrigin(0.5, 0.5);
            //sprite.setRotation((float) rotation*90);
        }
        place();
    };

    ////////////////////////////////////////////////////////////////////////////////

    virtual void getInteracted(){
        std::cout << "Mutterklasse" << std::endl;
    };

    ////////////////////////////////////////////////////////////////////////////////
    // Map-Functions

    int getMapsizeX(){
        return (uint64_t) map[0];
    }
    int getMapsizeY(){
        return (uint64_t) map[1];
    }

    Object** getNode(sf::Vector2<int> position){
        return &map[position.x * getMapsizeY() + position.y +2];
    }
    Object* getObject(sf::Vector2<int> position){
        return *getNode(position);
    }

protected:
    bool place(){
        printf("Placing Object to (%d, %d) \n", pos.x, pos.y);
    
        if(! isFree(pos, size)){
            // TODO: Dann woanders platzieren?
            std::cout << "ERROR: Position bereits belegt." << std::endl;
        }

        for (int x=0; x < size.x; x++){
            for (int y=0; y < size.y; y++){
                *getNode(pos + sf::Vector2i(x, y)) = this;
            }
        }
    };

    // Prüft, ob eine (Ziel-)position auf Map frei ist
    bool isFree(sf::Vector2<int> check_pos, sf::Vector2<int> offset){
        for (int x=0; x < offset.x; x++){
            for (int y=0; y < offset.y; y++){
                Object* obj = getObject(check_pos + sf::Vector2i(x, y));
                if(obj != nullptr && obj != this){
                    return false;
                }
            }
        }
        return true;
    };

    // Objekte "teleportieren"
    bool move(sf::Vector2<int> set_pos){
        // map-ränder verkleben
        if(set_pos.x < 0) set_pos.x = getMapsizeX() +set_pos.x;
        if(set_pos.y < 0) set_pos.y = getMapsizeY() +set_pos.y;
        if(set_pos.x >= getMapsizeX()) set_pos.x = set_pos.x - getMapsizeX();
        if(set_pos.y >= getMapsizeY()) set_pos.y = set_pos.y - getMapsizeY();

        //printf("Moving Object to (%d, %d) \n", set_pos.x, set_pos.y);
        
        if(! isFree(set_pos, size)){
            std::cout << "ERROR: Position bereits belegt." << std::endl;
            return false;
        } 

        // Befreie alte Pos.
        for (int x=0; x < size.x; x++){
            for (int y=0; y < size.y; y++){
                *getNode(pos + sf::Vector2i(x, y)) = nullptr;
            }
        }

        // Setze neue Pos.
        for (int x=0; x < size.x; x++){
            for (int y=0; y < size.y; y++){
                *getNode(set_pos + sf::Vector2i(x, y)) = this;
            }
        }
        
        pos.x = set_pos.x;
        pos.y = set_pos.y;

        return true;
    };

};

////////////////////////////////////////////////////////////////////////////////

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

////////////////////////////////////////////////////////////////////////////////

class LivingObject : public Object {
public:
    sf::Vector2<int> dir; //direction in which the object is looking
    LivingObject(Object ** map, int x, int y, int sx=2, int sy=2, sf::Vector2<int> d = DOWN, Tex* tex=nullptr) : 
        Object(map, x, y, sx, sy, tex), dir(d)
    {};

    bool stepRight(){
        // vorerst über moveObj() -> Bei größeren objekten ist es aber sinnvoller, nicht alle Felder jedes Mal neu zu belegen, sondern immer nur das erste Feld in die zu bewegende richtung.
        //lookInDirection('r');
        return move(pos + RIGHT);
    };

    bool stepLeft(){
        // vorerst über moveObj() -> Bei größeren objekten ist es aber sinnvoller, nicht alle Felder jedes Mal neu zu belegen, sondern immer nur das erste Feld in die zu bewegende richtung.
        //lookInDirection('l');
        return move(pos + LEFT);
    };

    bool stepUp(){
        // vorerst über moveObj() -> Bei größeren objekten ist es aber sinnvoller, nicht alle Felder jedes Mal neu zu belegen, sondern immer nur das erste Feld in die zu bewegende richtung.
        //lookInDirection('u');
        return move(pos + UP);
    };

    bool stepDown(){
        // vorerst über moveObj() -> Bei größeren objekten ist es aber sinnvoller, nicht alle Felder jedes Mal neu zu belegen, sondern immer nur das erste Feld in die zu bewegende richtung.
        //lookInDirection('d');
        return move(pos + DOWN);
    };
};


////////////////////////////////////////////////////////////////////////////////

class Player : public LivingObject {
public:
    Player(Object ** map, int x, int y, Tex* tex) : 
        LivingObject(map, x, y, 3, 3, RIGHT, tex)
    {};

    bool interact(){
        if(isFree((pos+dir), size)) return false;
        Object* obj = getObject(pos+dir);
        std::cout << "ich stehe vor einem Objekt :)" << std::endl;
        obj->getInteracted();
    };
};

class Enemy : public LivingObject {

};



////////////////////////////////////////////////////////////////////////////////

class Item : public Object {
public:
    Item (Object ** map, int x, int y, int sx, int sy, Tex* tex) : 
        Object(map, x, y, sx, sy, tex)
    {};
    bool collect(){
        // von Map entfernen
        // zu Bag hinzufügen
        return true;
    };
    bool use(){
        // aus Bag entfernen
        // Animation
        // Wirkung (auf alle Objekte im Wirkradius)
        return true;
    };
    void getInteracted() override{
        std::cout << "Huch, mit mir wurde interagiert." << std::endl;
    };
private:
    int distance; // Wie weit geht die Wirkung des Items, bis sie auf ein Objekt trifft?
    bool radial; // Falls true: Wirkung in alle Richtungen statt nur in Blickrichtung des Anwenders.
    //virtual void effect(); // Die Wirkung. In Itemtypen zu implementieren!
};

#endif //OBJECT_H
