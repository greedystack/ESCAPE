#ifndef OBJECT_H
#define OBJECT_H

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include "Animation.hpp"
#include <iostream>
#include <list>
#include <map>

// Ein Objekt auf der Map. IDEE: Könnte von Sprite erben.
// Problem: Da draw() von Sprite private... 

const sf::Vector2<int> RIGHT(1,0);
const sf::Vector2<int> LEFT(-1,0);
const sf::Vector2<int> UP(0,-1);
const sf::Vector2<int> DOWN(0,1);

class Object {
////////////////////////////////////////////////////////////////////////////////
///// STATIC
////////////////////////////////////////////////////////////////////////////////
protected:
    static std::map<std::string, Texsheet*> texsheets;
public:
    static void loadTexsheets(std::string theme = "standard"){
        texsheets["wall"] = new Texsheet("../include/img/wall0.png");
    }
////////////////////////////////////////////////////////////////////////////////

public:
    sf::Sprite sprite;
    sf::Vector2i pos, size, dir; // Position ist immer oben links am Objekt. Size ist dessen Größe in Map-Nodes.
    bool visible=false;
    
protected:
    Texsheet *tex;
    Object** map;

public:
    ////////////////////////////////////////////////////////////////////////////////
    //Object* r, l, u, d, on; // Ich bau den Müll jetzt in eine 2d-Liste um, wenn ich eh schon bei identisch großen Objekten bleibe.
    //Object** connected=nullptr; // das wäre dann ein Array für größere Objekte - die bestehen dann aus einelnen subobjekten.
    ////////////////////////////////////////////////////////////////////////////////

    Object(Object** m, int x, int y, Texsheet* t = nullptr, sf::Vector2i d=DOWN, int sx=1, int sy=1) 
        : map(m), pos(x, y), size(sx, sy), tex(t), dir(d)
    {
        if (tex != nullptr){
            visible=true;
            sprite.setTexture(tex->texture);

            int OBJECTUNIT = 20;
            sf::Vector2f scale(
                (float) (OBJECTUNIT * size.x) / tex->getSize().x, 
                (float) (OBJECTUNIT * size.y) / tex->getSize().y);
            sprite.scale(scale);
            
            //sprite.setRotation((float) rotation*90);
        }
        place();
    };

    ////////////////////////////////////////////////////////////////////////////////

    virtual bool getInteracted(Object* interacter){ // passive interaktion
        return false;
    };
    virtual bool interact(Object* interacter){ //aktive interaktion
        return false;
    };

    virtual void update(){ // macht virtual hier sinn?

    }

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

    // ACHTUNG!! Nur für 1x1-große Objekte gebaut! 
    Object* neighbor(sf::Vector2i _dir){
        return getObject(pos+_dir);
    };

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
    // ACHTUNG!! Entfernt Objekt nur von Map, nicht aus dem Speicher!
    void removeFromMap(){
        *getNode(pos) = nullptr;
        pos = sf::Vector2i(-1, -1);
    }
    // ACHTUNG!! Nur für 1x1-große Objekte gebaut! 
    void putOnMap(sf::Vector2i _pos){
        *getNode(_pos) = this;
        pos = _pos;
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
    Barrier(Object ** map, int x, int y) : 
        Object(map, x, y, texsheets["wall"])
    {};

};

////////////////////////////////////////////////////////////////////////////////

class Portal : public Object {
// Türen, Idee: statt aufschließen (permanent öffnen) kann man mit animation durchgehen, wenn man schlüssel hat. -> dann genau gleiche Funktion wie teleportationsportale.

};

////////////////////////////////////////////////////////////////////////////////

class LivingObject : public Object {
public:
    LivingObject(Object ** map, int x, int y, Texsheet* tex=nullptr, sf::Vector2i d = DOWN) : 
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
private:
    std::list<Object*> bag;
public:

    Player(Object ** map, int x, int y) : 
        LivingObject(map, x, y, texsheets["wall"], RIGHT)
    {};
    ~Player(){
        for (Object* i : bag) {
            free(i);
            std::cout << i << " from BAG deleted.\n";
        }
    }

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
};

////////////////////////////////////////////////////////////////////////////////

class Enemy : public LivingObject {

};
////////////////////////////////////////////////////////////////////////////////

class Item : public Object {
public:
    Item (Object ** map, int x, int y) : 
        Object(map, x, y, texsheets["wall"])
    {};
    bool collect(Player* collector){
        removeFromMap();
        collector->putInBag(this);
        return true;
    };
    virtual bool interact(Object* player) override{
        std::cout << "placing item" << this << std::endl;
        sf::Vector2i newpos = player->pos + player->dir;
        if(getObject(newpos) != nullptr) return false;
        pos = newpos;
        place();
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
///// Static vars ankündigen:
////////////////////////////////////////////////////////////////////////////////
std::map<std::string, Texsheet*> Object::texsheets;

#endif //OBJECT_H
