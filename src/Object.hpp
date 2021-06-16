#ifndef OBJECT_H
#define OBJECT_H

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include "Animation.hpp"
#include <iostream>
#include <list>
#include <set>
#include <map>

// Ein Objekt auf der Map. IDEE: Könnte von Sprite erben.
// Problem: Da draw() von Sprite private... 

const sf::Vector2i RIGHT(1,0);
const sf::Vector2i LEFT(-1,0);
const sf::Vector2i UP(0,-1);
const sf::Vector2i DOWN(0,1);

// Identifier for whoami()
const uint OBJECT = 0;
const uint ITEM = 1;
const uint BARRIER = 2;
const uint PORTAL = 3;
const uint GOAL = 31;
const uint DESTROYER = 11;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// LAYER 0
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
class Object {
////////////////////////////////////////////////////////////////////////////////
///// STATIC
////////////////////////////////////////////////////////////////////////////////
protected:
    static std::map<std::string, Texsheet*> texsheets;
public:
    static void loadTexsheets(std::string theme = "standard"){
        texsheets["wall"] = new Texsheet("../include/img/wall0.png");
        texsheets["arrow_left"] = new Texsheet("../include/img/arrow.png", 1, 4);
    }
////////////////////////////////////////////////////////////////////////////////

public:
    sf::Sprite sprite;
    sf::Vector2i pos, size, dir; // Position ist immer oben links am Objekt. Size ist dessen Größe in Map-Nodes.
    bool visible=false;
    
protected:
    Texsheet *tex;
    Object** map;
    int OBJECTUNIT = 20;
    std::set<uint> identity;

public:
    ////////////////////////////////////////////////////////////////////////////////
    //Object* r, l, u, d, on; // Ich bau den Müll jetzt in eine 2d-Liste um, wenn ich eh schon bei identisch großen Objekten bleibe.
    //Object** connected=nullptr; // das wäre dann ein Array für größere Objekte - die bestehen dann aus einelnen subobjekten.
    ////////////////////////////////////////////////////////////////////////////////

    Object(Object** m, int x, int y, Texsheet* t = nullptr, sf::Vector2i d=LEFT, int sx=1, int sy=1) 
        : map(m), pos(x, y), size(sx, sy), tex(t)
    {
        identity.insert(OBJECT);
        if (tex != nullptr){
            visible=true;
            sprite.setTexture(tex->texture);
            //sprite.setOrigin(10.f,10.f); // weird!
            
            setDirection(d);
            sf::Vector2f scale(
                (float) (OBJECTUNIT * size.x) / tex->getSize().x,
                (float) (OBJECTUNIT * size.y) / tex->getSize().y);
            sprite.scale(scale);
            
        }
        place();
        sprite.setPosition((float)pos.x*OBJECTUNIT, (float)pos.y*OBJECTUNIT);
    };

    std::set<uint> whoami(){return identity;}

    ////////////////////////////////////////////////////////////////////////////////

    virtual bool getInteracted(Object* interacter){ // passive interaktion
        return false;
    };
    virtual bool interact(Object* interactee){ //aktive interaktion
        return false;
    };

    virtual void update(){ // Fkt. für Animationen und draw und so. Macht virtual hier sinn?

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

    // ACHTUNG!! Nur für 1x1-große Objekte gebaut! 
    // Entfernt Objekt restlos.
    void del(){
        // TODO WARUM GEHT DAS NICHT PROTECTED?????
        removeFromMap();
        delete this;
    };

protected:
    bool place(){
        //printf("Placing Object to (%d, %d) \n", pos.x, pos.y);
    
        if(! isFree(pos, size)){
            // TODO: Dann woanders platzieren?
            std::cout << "ERROR: Position bereits belegt." << std::endl;
        }

        for (int x=0; x < size.x; x++){
            for (int y=0; y < size.y; y++){
                *getNode(pos + sf::Vector2i(x, y)) = this;
            }
        }
        return true;
    };

    

    // ACHTUNG!! Nur für 1x1-große Objekte gebaut! 
    // ACHTUNG!! Entfernt Objekt nur von Map, nicht aus dem Speicher!
    void removeFromMap(){
        *getNode(pos) = nullptr;
        pos = sf::Vector2i(-1, -1);
    };
    
    // ACHTUNG!! Nur für 1x1-große Objekte gebaut! 
    void putOnMap(sf::Vector2i _pos){
        *getNode(_pos) = this;
        pos = _pos;
    };

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
    bool teleport(sf::Vector2<int> set_pos){
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
        sprite.setPosition((float)pos.x*OBJECTUNIT, (float)pos.y*OBJECTUNIT);

        return true;
    };

    void setTexType(int row){
        if(tex->getImageCount().y <= row){
            std::cout << "ERROR: Texsheet hat nicht so viele Modi. Setze Modus 0.";
            row = 0;
        }
        sf::Vector2i texsize = (sf::Vector2i) tex->getSize();
        sf::Vector2i position(0, texsize.y * row);
        sprite.setTextureRect(sf::IntRect(position,texsize));
        // std::cout << "Texsize: " << texsize.x << ", " << texsize.y * row << std::endl;
    };

    void setDirection(sf::Vector2i _dir){
        if(dir == _dir) return;
        int row;
        if(_dir == LEFT) row = 0;
        if(_dir == UP) row =1;
        if(_dir == RIGHT) row = 2;
        if(_dir == DOWN) row = 3;

        setTexType(row);
        dir = _dir;
    };

};
////////////////////////////////////////////////////////////////////////////////
///// Static vars ankündigen:
////////////////////////////////////////////////////////////////////////////////
std::map<std::string, Texsheet*> Object::texsheets;

#endif //OBJECT_H
