#ifndef LEVEL_H
#define LEVEL_H

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics.hpp>
#include "Tex.h"
#include "Object.h"

// Stellt ein Level dar und verwaltet die Map etc.

class Level {
public:
    Level(int, int);
    ~Level();
    
    Object** getNode(int, int);
    Object* getObject(int, int);

    // OLD
    
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

    bool moveObj(Object*, int, int);
    bool stepRight(Object*);
    bool stepLeft(Object*);
    bool stepUp(Object*);
    bool stepDown(Object*);

private:
    const int OBJECTUNIT = 20; // ACHTUNG AUCH NOCHMAL IM GAME FÜR DEN RENDERER DEFINIERT!
    Object** map;
    Object* player;
    int mapsizex, mapsizey;

    Object* createObj(int, int, int=1, int=1);
    bool isFree(int, int, int, int, Object* = nullptr);

    void buildBorders();
    void drawBackground(int);
};



#endif //LEVEL_H