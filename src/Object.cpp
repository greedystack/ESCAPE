#include "Object.h"

Object::Object(Object** m, int setX, int setY, int sx, int sy, Tex* t) 
    : map(m), posX(setX), posY(setY), sizeX(sx), sizeY(sy), tex(t)
{
    
    if (tex != nullptr){
        visible=true;
        sprite.setTexture(tex->regular);
        sprite.setOrigin(0.5, 0.5);
        //sprite.setRotation((float) rotation*90);
    }

    printf("Creating Object to (%d, %d) \n", setX, setY);
    printf("MsX: %d \t MsY: %d\n\n", getMapsizeX() , getMapsizeY());
    
    if(! isFree(setX, setY, sizeX, sizeY)){
        // @TODO
    }

    for (int x=0; x < sizeX; x++){
        for (int y=0; y < sizeY; y++){
            *getNode(setX+x, setY+y) = this;
        }
    }
}


// Achtung! textur invertieren funktioniert nicht wirklich. Zeigt komisches Verhalten: Objekt bleibt bei blickrichtungswechsel auf der Stelle stehen, obwohl sich Position in Map ändert
void Object::lookInDirection(char dir){
    switch (dir){
        case 'r':
            sprite.setScale(tex->scaleX * (-1), tex->scaleY);
            break;
        case 'l':
            sprite.setScale(tex->scaleX, tex->scaleY);
            break;
        case 'u':
            // Hier fehlen noch Textures
            break;
        default:
        case 'd':
            // Hier fehlen noch Textures
            break;
    }
}

////////////////////////////////////////////////////////////////////////////////
// Map-Functions

int Object::getMapsizeX(){
    return (uint64_t) map[0];
}
int Object::getMapsizeY(){
    return (uint64_t) map[1];
}

Object** Object::getNode(int x, int y){
    return &map[x * getMapsizeY() + y +2];
}
Object* Object::getObject(int x, int y){
    return *getNode(x, y);
}

// Prüft, ob eine (Ziel-)position auf Map frei ist
bool Object::isFree(int pos_x, int pos_y, int offset_x, int offset_y){
    for (int x=0; x < offset_x; x++){
        for (int y=0; y < offset_y; y++){
            if(getObject(pos_x+x, pos_y+y) != nullptr && getObject(pos_x+x, pos_y+y) != this){
                printf("ERROR: Zielposition bereits belegt! \n");
                return false;
            }
        }
    }
    return true;
}

// Objekte "teleportieren"
bool Object::move(int setX, int setY){
    // map-ränder verkleben
    if(setX < 0) setX = getMapsizeX() +setX;
    if(setY < 0) setY = getMapsizeY() +setY;
    if(setX >= getMapsizeX()) setX = setX - getMapsizeX();
    if(setY >= getMapsizeY()) setY = setY - getMapsizeY();

    //printf("Moving Object to (%d, %d) \n", setX, setY);
    
    if(! isFree(setX, setY, sizeX, sizeY)) return false;

    // Befreie alte Pos.
    for (int x=0; x < sizeX; x++){
        for (int y=0; y < sizeY; y++){
            *getNode(posX +x, posY +y) = nullptr;
        }
    }

    // Setze neue Pos.
    for (int x=0; x < sizeX; x++){
        for (int y=0; y < sizeY; y++){
            *getNode(setX+x, setY+y) = this;
        }
    }
    
    posX = setX;
    posY = setY;

    return true;
}

////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////

bool Object::stepRight(){
    // vorerst über moveObj() -> Bei größeren objekten ist es aber sinnvoller, nicht alle Felder jedes Mal neu zu belegen, sondern immer nur das erste Feld in die zu bewegende richtung.
    //lookInDirection('r');
    return move(posX+1, posY);
}

bool Object::stepLeft(){
    // vorerst über moveObj() -> Bei größeren objekten ist es aber sinnvoller, nicht alle Felder jedes Mal neu zu belegen, sondern immer nur das erste Feld in die zu bewegende richtung.
    //lookInDirection('l');
    return move(posX-1, posY);
}

bool Object::stepUp(){
    // vorerst über moveObj() -> Bei größeren objekten ist es aber sinnvoller, nicht alle Felder jedes Mal neu zu belegen, sondern immer nur das erste Feld in die zu bewegende richtung.
    lookInDirection('u');
    return move(posX, posY-1);
}

bool Object::stepDown(){
    // vorerst über moveObj() -> Bei größeren objekten ist es aber sinnvoller, nicht alle Felder jedes Mal neu zu belegen, sondern immer nur das erste Feld in die zu bewegende richtung.
    lookInDirection('d');
    return move(posX, posY+1);
}