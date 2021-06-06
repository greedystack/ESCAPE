#ifndef MAP_HPP
#define MAP_H

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

class Map {
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

#endif //MAP_HPP
