#include "Level.h"
#include "Tex.h"





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


void Level::buildBorders(){
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