#ifndef OBJECT_H
#define OBJECT_H

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include "Tex.h"

// Ein Objekt auf der Map. IDEE: Könnte von Sprite erben.

class Object {
public:
    Object(Tex*); // eigentlich wärs ja besser, wenn der direkt ein Array seiner Standpositions-Nodes bekommt

    sf::Sprite sprite;
    int posX, posY, sizeX, sizeY; // Position ist immer oben links am Objekt. Size ist dessen Größe in Map-Nodes.

    // Getter
    int x(){return posX;};
    int y(){return posY;};
    int sizex(){return sizeX;};
    int sizey(){return sizeY;};
    bool isVisible(){return visible;};
    void lookInDirection(char);

private:
    
    bool visible=false;
    Tex *tex;
};

#endif //OBJECT_H
