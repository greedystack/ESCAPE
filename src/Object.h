#ifndef OBJECT_H
#define OBJECT_H

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include "Tex.h"

class Object : public sf::Sprite {
public:
    Object(Tex*, int, int, int, int); // eigentlich wärs ja besser, wenn der direkt ein Array seiner Standpositions-Nodes bekommt

    sf::Sprite sprite;

    // Getter
    int x(){return posX;};
    int y(){return posY;};
    int sizex(){return sizeX;};
    int sizey(){return sizeY;};
    bool isVisible(){return visible;};

private:
    int posX, posY, sizeX, sizeY; // Position ist immer oben links am Objekt. Size ist dessen Größe in Map-Nodes.
    bool visible=false;
    Tex *tex;
};

#endif //OBJECT_H
