//
// Created by Marius on 08.01.20.
//

#ifndef FIRST_GAME_H
#define FIRST_GAME_H

#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Font.hpp>
#include "MapObject.h"

class Game {
public:
    Game(uint64_t, uint64_t, int);
    void start();

private:
    uint64_t mapsizex, mapsizey;
    int playercount;
    const int HEADER = 2;
    const int TEXTURES_PER_CATEGORY = 100;
    MapObject **map;
    MapObject **player;
    Tex **textures;
    sf::Font font;
    Tex*  getTexture(char c, int i){return textures[(uint16_t)c+i];};
    void loadTexture(char c, int i, Tex* t){textures[(uint16_t)c+i] = t;};
    void loadTextures();
    void loadFonts();

    void render();
    void catchEvents();
    void catchKeypress();
    sf::RenderWindow *window;

    void buildBorders();
    void drawBackground(int);
};


#endif //FIRST_GAME_H
