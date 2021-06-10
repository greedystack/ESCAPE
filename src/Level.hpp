#ifndef LEVEL_H
#define LEVEL_H

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics.hpp>

#include <map>

#include "Texsheet.hpp"
#include "Object.hpp"

// Stellt ein Level dar und verwaltet die Map etc.

class Level {
public:
    // OLD
    sf::Font font;
    //////////////////////////////////////////////////////////////////////////////////////////////
private:
    Object** map;
    Player* player;
    int mapsizex, mapsizey;

    
public:
    
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

        Object::loadTexsheets();

        // OLD:
        // textures = (Tex**)(malloc(5 * sizeof(uint8_t) * sizeof(Tex *)));
        //loadTextures();
        loadFonts();
        ////

        new Item(map, 10, 3);
        new Item(map, 10, 6);
        player = new Player(map, 4, 1);
        new Barrier(map, 3, 0);
        
        
        new Barrier(map, 0, 0);
        new Barrier(map, 0, 1);
        new Barrier(map, 0, 2);
        new Barrier(map, 1, 2);
        new Barrier(map, 2, 2);
        new Barrier(map, 3, 3);
        new Barrier(map, 4, 3);
        new Barrier(map, 5, 3);
        new Barrier(map, 6, 3);
        
        buildBorders();
        zinken();
    }

    ~Level(){
        for(int x = 0; x < mapsizex; x++){
            for(int y = 0; y < mapsizey; y++){
                Object* obj = getObject(sf::Vector2u(x, y));
                if(obj != nullptr){
                    delete obj;
                    //std::cout << "deleted object on ("<<x<<" , "<<y<<").\n";
                }
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



    Object** getNode(sf::Vector2u position){
        if(position.x*position.y >= mapsizex*mapsizey +2
            || position.x*position.y < 0
        ){
            std::cout<< "ERROR: Position (" << position.x <<", "<< position.y <<") out of Array!\n";
            return nullptr;
        }
        return &map[position.x * mapsizey + position.y +2];
    }
    Object* getObject(sf::Vector2u position){
        if(position.x*position.y >= mapsizex*mapsizey +2
            || position.x*position.y < 0
        ){
            std::cout<< "ERROR: Position (" << position.x <<", "<< position.y <<") out of Array!\n";
            return nullptr;
        }
        return *getNode(position);
    }


private:

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

void loadFonts() {
    if (!font.loadFromFile("../include/KulimPark-Regular.ttf"))
        printf("ERROR: FONT NOT LOADED!");
        //return EXIT_FAILURE;
}

////////////////////////////////////////////////////////////////////////////////


void buildBorders(){
    for (int x=0; x<mapsizex; x++){
        new Barrier(map, x, 0);
        new Barrier(map, x, mapsizey-1);
    }
    for (int y=1; y<mapsizey-1; y++){
        new Barrier(map, 0, y);
        new Barrier(map, mapsizex-1, y);
    }
}

void zinken(){
    for (int x=1; x<mapsizex-1; x+=2){
        new Barrier(map, x, 1);
        new Barrier(map, x, mapsizey-2);
    }
    for (int y=2; y<mapsizey-2; y+=2){
        new Barrier(map, 1, y);
        new Barrier(map, mapsizex-2, y);
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
};




#endif //LEVEL_H