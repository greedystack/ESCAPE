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
    Object(Tex* = nullptr);

    sf::Sprite sprite;
    int posX, posY, sizeX, sizeY; // Position ist immer oben links am Objekt. Size ist dessen Größe in Map-Nodes.
    bool visible=false;
    Tex *tex;

    void lookInDirection(char);

private:

};

class Wall : Object {

};

#endif //OBJECT_H
