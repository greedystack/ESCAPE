#include "Level.h"
#include "Tex.h"

Level::Level(int x, int y)
    : mapsizex(x), mapsizey(y)
{
    map = (Object**)(malloc(x * y * sizeof(Object *)));

    for(int i=0; i<(x*y); i++){
        map[i] = nullptr;
    }

    printf("Map allocated \n");

    // OLD:
    textures = (Tex**)(malloc(5 * sizeof(uint8_t) * sizeof(Tex *)));
    loadTextures();
    loadFonts();
    printf("Stuff der hier gar nicht hingehört loaded! \n");
    ////

    placeObj(new Object(getTexture('P', 0),1 , 1), 1, 1);

    // buildBorders();
    //new Object(getTexture('P', 0), 'W', 7, 7);
    //new Object(getTexture('P', 0), 'Q', 3, 3);
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
    loadTexture('b', 0, new Tex("../include/img/bg1.jpg", OBJECTUNIT, OBJECTUNIT)); // Testbackground

    // Player
    loadTexture('P', 0, new Tex("../include/img/mario.png", 20, 40));
}

void Level::loadFonts() {
    if (!font.loadFromFile("../include/KulimPark-Regular.ttf"))
        printf("ERROR: FONT NOT LOADED!");
        //return EXIT_FAILURE;
}


////////////////////////////////////////////////////////////////////////////////

Object** Level::getNode(int x, int y){
    return &map[x * mapsizey + y];
}
Object* Level::getObject(int x, int y){
    return *getNode(x, y);
}

////////////////////////////////////////////////////////////////////////////////

// IDEE: createObject() mit Übergabe von Objecttype!

bool Level::placeObj(Object obj, int setX, int setY, int dir){
    printf("Placing Object to (%d, %d) \n", setX, setY);
    
    // Prüfe ob Bewegung erlaubt
    for (int x=0; x < obj.x(); x++){
        for (int y=0; y < obj.y(); y++){
            if(getObject(setX+x, setY+y) != nullptr){
                printf("ERROR: Zielposition bereits belegt! \n");
                return false;
            }
        }
    }

    // Bewege
    for (int x=0; x < obj.x(); x++){
        for (int y=0; y < obj.y(); y++){
            *getNode(setX+x, setY+y) = &obj;
        }
    }

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