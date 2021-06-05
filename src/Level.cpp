#include "Level.h"
#include "Tex.h"

Level::Level(int x, int y)
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

    player = new Player(map, 5, 3, getTexture('P', 0));
    new Barrier(map, 0, 0, getTexture('w', 0));
    new Barrier(map, 0, 1, getTexture('w', 0));
    new Barrier(map, 0, 2, getTexture('w', 0));
    new Barrier(map, 1, 2, getTexture('w', 0));
    new Barrier(map, 2, 2, getTexture('w', 0));
    new Barrier(map, 3, 3, getTexture('w', 0));
    
    // buildBorders();
}

Level::~Level(){
    for(int i = 0; i < mapsizex*mapsizey; i++){
        if(&map[i] != nullptr){
            free(&map[i]);
        }
    }
    free(map);
    map = nullptr;
    printf("Map freed \n");
}

Object** Level::getNode(int x, int y){
    return &map[x * mapsizey + y +2];
}
Object* Level::getObject(int x, int y){
    return *getNode(x, y);
}

////////////////////////////////////////////////////////////////////////////////
// sf::Text text("Hello SFML", font, 30);
/*/ Load a music to play
        sf::Music music;
        if (!music.openFromFile("nice_music.ogg"))
            return EXIT_FAILURE;
        // Play the music
        music.play();
         */

void Level::loadTextures(){
    // backgrounds
    loadTexture('w', 0, new Tex("../include/img/wall0.png", OBJECTUNIT, OBJECTUNIT));

    // Player
    loadTexture('P', 0, new Tex("../include/img/mario.png", 20, 40));
}

void Level::loadFonts() {
    if (!font.loadFromFile("../include/KulimPark-Regular.ttf"))
        printf("ERROR: FONT NOT LOADED!");
        //return EXIT_FAILURE;
}

////////////////////////////////////////////////////////////////////////////////
/*

void Level::buildBorders(){
    for (int x=0; x<mapsizex; x++){
        placeObj(new Object, x, 0);
        placeObj(new Object, x, mapsizey-1);
    }
    for (int y=1; y<mapsizey-1; y++){
        placeObj(new Object, 0, y);
        placeObj(new Object, mapsizex-1, y);
    }
}

void Level::drawBackground(int bgRepeteAfter){
    for (int x=0; x<bgRepeteAfter; x++){
        for (int y=0; y<bgRepeteAfter; y++){
            new Object(getTexture('b', 0), ' ', x, y);
        }
    }
}
*/