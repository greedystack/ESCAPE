#ifndef LEVEL_H
#define LEVEL_H

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics.hpp>


#include "Tex.h"
#include "Object.hpp"

// Stellt ein Level dar und verwaltet die Map etc.

class Level {
public:
    // OLD
    
    Tex **textures;
    sf::Font font;
    Tex*  getTexture(char c, int i){return textures[(uint16_t)c+i];};
    void loadTexture(char c, int i, Tex* t){textures[(uint16_t)c+i] = t;};

    //////////////////////////////////////////////////////////////////////////////////////////////
    
    Level(uint64_t x, uint64_t y)
        : mapsizex(x), mapsizey(y)
    {
        map = (Object**)(malloc((2 + x * y) * sizeof(Object *)));

        for(int i=0; i<(x*y); i++){
            map[i] = nullptr;
        }

        map[0] = (Object*)x;
        map[1] = (Object*)y;

        printf("Map allocated \n");

        // OLD:
        textures = (Tex**)(malloc(5 * sizeof(uint8_t) * sizeof(Tex *)));
        loadTextures();
        loadFonts();
        ////

        

        new LivingObject(map, 10, 10, getTexture('w', 0), LEFT);
        new Item(map, 10, 3, getTexture('w', 0));
        player = new Player(map, 7, 4, getTexture('w', 0));
        
        
        new Barrier(map, 0, 0, getTexture('w', 0));
        new Barrier(map, 0, 1, getTexture('w', 0));
        new Barrier(map, 0, 2, getTexture('w', 0));
        new Barrier(map, 1, 2, getTexture('w', 0));
        new Barrier(map, 2, 2, getTexture('w', 0));
        new Barrier(map, 3, 3, getTexture('w', 0));
        new Barrier(map, 4, 3, getTexture('w', 0));
        new Barrier(map, 5, 3, getTexture('w', 0));
        new Barrier(map, 6, 3, getTexture('w', 0));
        
        //buildBorders();
    }

    ~Level(){
        for(int i = 0; i < mapsizex*mapsizey; i++){
            if(&map[i] != nullptr){
                delete &map[i];
            }
        }
        free(map);
        map = nullptr;
        printf("Map freed \n");
    }
    
    //////////////////////////////////////////////////////////////////////////////////////////////

    Player* getPlayer(){return player;};
    int getMapX(){return mapsizex;};
    int getMapY(){return mapsizey;};



    Object** getNode(sf::Vector2<int> position){
        return &map[position.x * mapsizey + position.y +2];
    }
    Object* getObject(sf::Vector2<int> position){
        return *getNode(position);
    }

//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// sf::Text text("Hello SFML", font, 30);
/*/ Load a music to play
        sf::Music music;
        if (!music.openFromFile("nice_music.ogg"))
            return EXIT_FAILURE;
        // Play the music
        music.play();
         */

void loadTextures(){
    // backgrounds
    loadTexture('w', 0, new Tex("../include/img/wall0.png", OBJECTUNIT, OBJECTUNIT));

    // Player
    loadTexture('P', 0, new Tex("../include/img/mario.png", 20, 40));
}

void loadFonts() {
    if (!font.loadFromFile("../include/KulimPark-Regular.ttf"))
        printf("ERROR: FONT NOT LOADED!");
        //return EXIT_FAILURE;
}

////////////////////////////////////////////////////////////////////////////////


void buildBorders(){
    for (int x=0; x<mapsizex; x++){
        new Barrier(map, x, 0, getTexture('w', 0));
        new Barrier(map, x, mapsizey-1, getTexture('w', 0));
    }
    for (int y=1; y<mapsizey-1; y++){
        new Barrier(map, 0, y, getTexture('w', 0));
        new Barrier(map, mapsizex-1, y, getTexture('w', 0));
    }
}

/*

void Level::drawBackground(int bgRepeteAfter){
    for (int x=0; x<bgRepeteAfter; x++){
        for (int y=0; y<bgRepeteAfter; y++){
            new Object(getTexture('b', 0), ' ', x, y);
        }
    }
}
*/

private:
    const int OBJECTUNIT = 20; // ACHTUNG AUCH NOCHMAL IM GAME FÜR DEN RENDERER DEFINIERT!
    Object** map;
    Player* player;
    int mapsizex, mapsizey;

    void drawBackground(int);
};




#endif //LEVEL_H