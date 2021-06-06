#ifndef OBJECT_H
#define OBJECT_H

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include "Tex.h"
#include<iostream>
#include <list>

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
    sf::Vector2i pos, size, dir; // Position ist immer oben links am Objekt. Size ist dessen Größe in Map-Nodes.

    bool visible=false;
    Tex *tex;
    ////////////////////////////////////////////////////////////////////////////////

    Object(Object** m, int x, int y, Tex* t = nullptr, sf::Vector2i d=DOWN, int sx=1, int sy=1) 
        : map(m), pos(x, y), size(sx, sy), tex(t), dir(d)
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

    virtual bool getInteracted(Object* interacter){
        return false;
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

    // ACHTUNG!! Nur für 1x1-große Objekte gebaut! 
    Object* neighbor(sf::Vector2i _dir){
        return getObject(pos+_dir);
    };

    // ACHTUNG!! Nur für 1x1-große Objekte gebaut! 
    // ACHTUNG!! Entfernt Objekt nur von Map, nicht aus dem Speicher!
    void removeFromMap(){
        *getNode(pos) = nullptr;
    }

    // Prüft, ob eine (Ziel-)position auf Map frei ist
    bool isFree(sf::Vector2i check_pos, sf::Vector2i offset){
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
        Object(map, x, y, tex)
    {};

};

class Portal : public Object {
// Türen, Idee: statt aufschließen (permanent öffnen) kann man mit animation durchgehen, wenn man schlüssel hat. -> dann genau gleiche Funktion wie teleportationsportale.

};

////////////////////////////////////////////////////////////////////////////////

class LivingObject : public Object {
public:
    LivingObject(Object ** map, int x, int y, Tex* tex=nullptr, sf::Vector2i d = DOWN) : 
        Object(map, x, y, tex, d)
    {};

    // nur genau einen Schritt moven
    bool step(sf::Vector2i _dir){
         // vorerst über moveObj() -> Bei größeren objekten ist es aber sinnvoller, nicht alle Felder jedes Mal neu zu belegen, sondern immer nur das erste Feld in die zu bewegende richtung.
        dir = _dir;
        return move(pos + _dir);
    };
};

////////////////////////////////////////////////////////////////////////////////
class Player : public LivingObject {
public:

    Player(Object ** map, int x, int y, Tex* tex) : 
        LivingObject(map, x, y, tex, RIGHT)
    {};
    ~Player(){
        for (Object* i : bag) {
            delete i;
        }
    }

    // TODO Hier und an der neighbor() scheitert aktuell die variable Objektgröße. Fix this!
    // z.B. fixbar durch loop, in der dann mit alllen adjazenten, interactable Objects interagiert wird.
    bool interact(){
        Object* nb = neighbor(dir);
        if(nb == nullptr) return false;
        return nb->getInteracted(this);
    };
    void putInBag(Object* item){
        bag.push_back(item);
    };

private:
    std::list<Object*> bag;
};

class Enemy : public LivingObject {

};
////////////////////////////////////////////////////////////////////////////////

class Item : public Object {
public:
    Item (Object ** map, int x, int y, Tex* tex) : 
        Object(map, x, y, tex)
    {};
    bool collect(Player* collector){
        removeFromMap();
        collector->putInBag(this);
        return true;
    };
    bool use(){
        // aus Bag entfernen??
        // Animation
        // Wirkung (auf alle Objekte im Wirkradius)
        return true;
    };
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






#endif //OBJECT_H
