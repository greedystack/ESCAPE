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
    ////

    createObj(5, 5);
    player = createObj(2, 2);
    
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

Object** Level::getNode(int x, int y){
    return &map[x * mapsizey + y];
}
Object* Level::getObject(int x, int y){
    return *getNode(x, y);
}

////////////////////////////////////////////////////////////////////////////////
// Prüft, ob Zielposition frei ist
// Der Object-Parameter ist per default nullptr - wenn gesetzt, ist dies die erlaubte Objectinstanz, die sich auf dem Feld befinden darf (da gebe ich am schlauesten das zu bewegende objekt selbst an)
bool Level::isFree(int pos_x, int pos_y, int offset_x, int offset_y, Object* obj){
    for (int x=0; x < offset_x; x++){
        for (int y=0; y < offset_y; y++){
            if(getObject(pos_x+x, pos_y+y) != nullptr && getObject(pos_x+x, pos_y+y) != obj){
                printf("ERROR: Zielposition bereits belegt! \n");
                return false;
            }
        }
    }
    return true;
}


// "Konstruktor" für neue Objekte. Platziert Objekte direkt in der Map
// IDEE: createObject() mit Übergabe von Objecttype!
Object* Level::createObj(int setX, int setY, int sizeX, int sizeY){
    printf("Creating Object to (%d, %d) \n", setX, setY);
    
    if(! isFree(setX, setY, sizeX, sizeY)) return nullptr;

    Object *obj = new Object(getTexture('P', 0));
    obj->posX = setX;
    obj->posY = setY;
    obj->sizeX = sizeX;
    obj->sizeY = sizeY;

    for (int x=0; x < obj->sizeX; x++){
        for (int y=0; y < obj->sizeY; y++){
            *getNode(setX+x, setY+y) = obj;
        }
    }
    return obj;
}

// Objekte teleportieren
bool Level::moveObj(Object *obj, int setX, int setY){
    // ränder teleportieren
    if(setX < 0) setX = mapsizex +setX;
    if(setY < 0) setY = mapsizey +setY;
    if(setX >= mapsizex) setX = setX - mapsizex;
    if(setY >= mapsizey) setY = setY - mapsizey;

    printf("Moving Object to (%d, %d) \n", setX, setY);
    
    if(! isFree(setX, setY, obj->sizeX, obj->sizeY, obj)) return false;

    // Bewege
    for (int x=0; x < obj->sizeX; x++){
        for (int y=0; y < obj->sizeY; y++){
            *getNode(obj->posX +x, obj->posY +y) = nullptr;
            *getNode(setX+x, setY+y) = obj;
        }
    }
    obj->posX = setX;
    obj->posY = setY;

    return true;
}

////////////////////////////////////////////////////////////////////////////////

bool Level::stepRight(Object* obj){
    // vorerst über moveObj() -> Bei größeren objekten ist es aber sinnvoller, nicht alle Felder jedes Mal neu zu belegen, sondern immer nur das erste Feld in die zu bewegende richtung.
    obj->lookInDirection('r');
    return moveObj(obj, obj->posX +1, obj->posY);
}

bool Level::stepLeft(Object* obj){
    // vorerst über moveObj() -> Bei größeren objekten ist es aber sinnvoller, nicht alle Felder jedes Mal neu zu belegen, sondern immer nur das erste Feld in die zu bewegende richtung.
    obj->lookInDirection('l');
    return moveObj(obj, obj->posX -1, obj->posY);
}

bool Level::stepUp(Object* obj){
    // vorerst über moveObj() -> Bei größeren objekten ist es aber sinnvoller, nicht alle Felder jedes Mal neu zu belegen, sondern immer nur das erste Feld in die zu bewegende richtung.
    obj->lookInDirection('u');
    return moveObj(obj, obj->posX, obj->posY -1);
}

bool Level::stepDown(Object* obj){
    // vorerst über moveObj() -> Bei größeren objekten ist es aber sinnvoller, nicht alle Felder jedes Mal neu zu belegen, sondern immer nur das erste Feld in die zu bewegende richtung.
    obj->lookInDirection('d');
    return moveObj(obj, obj->posX, obj->posY +1);
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