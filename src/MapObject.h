//
// Created by Marius on 02.01.20.
//

#ifndef FIRST_MAPOBJECT_H
#define FIRST_MAPOBJECT_H


#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include "Tex.h"


class MapObject {
public:
    sf::Sprite sprite;
    bool visible=false;

    MapObject(MapObject** m, Tex* tex, char t=' ', int x=0, int y=0, int size_x=1, int size_y=1);

    bool isVisible(){return visible;};

    void setColor(sf::Color c = sf::Color(0,0,0,0)){sprite.setColor(c);};

    bool isMovable(){
        if(type == 'P') return true;
        return false;
    };
    
    MapObject** getMapNode(int x, int y);
    MapObject* getMapObject(int x, int y);

    bool setPos(int, int);
    void unsetPos();
    bool teleport(int, int);
    bool moveRight();
    bool moveLeft();
    bool moveUp();
    bool moveDown();

    int getX(){return posX;};
    int getY(){return posY;};

private:
    char type;
    int posX, posY, sizeX, sizeY; // Position ist immer oben links am Objekt. Size ist dessen Größe in Map-Nodes. 

    MapObject** map; // TODO: Nur, um auf Map zugreifen zu können. Geht das nicht auch als globele Var des Games?
    Tex* texture;
};

#endif //FIRST_MAPOBJECT_H
