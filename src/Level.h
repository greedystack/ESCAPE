#ifndef LEVEL_H
#define LEVEL_H

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics.hpp>
#include "Tex.h"
#include "Object.h"

class Level {
public:
    Level(int, int);
    ~Level();
    
    Object** getNode(int, int);
    Object* getObject(int, int);

    // OLD
    const int OBJECTUNIT = 20;
    Tex **textures;
    sf::Font font;
    Tex*  getTexture(char c, int i){return textures[(uint16_t)c+i];};
    void loadTexture(char c, int i, Tex* t){textures[(uint16_t)c+i] = t;};
    void loadTextures();
    void loadFonts();
    //////////

    Object* getPlayer(){return player;};
    int getMapX(){return mapsizex;};
    int getMapY(){return mapsizey;};

private:
    char type;
    Object** map;
    Object* player;
    int mapsizex, mapsizey;

    bool placeObj(Object, int, int, int=0);

    void buildBorders();
    void drawBackground(int);
};



#endif //LEVEL_H