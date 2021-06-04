#include "Object.h"

Object::Object(Tex* t) 
    : tex(t)
{
    sizeX =1;
    sizeY =1;
    if (tex != nullptr){
        visible=true;
        sprite.setTexture(tex->regular);
        sprite.setOrigin(0.5, 0.5);
        //sprite.setRotation((float) rotation*90);
    }
}

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

/*
Object** Object::getNode(int x, int y){
    return &map[x * map[0] + y +1];
}
Object* Object::getObject(int x, int y){
    return *getNode(x, y);
}



bool Object::setPos(int setX, int setY){
    printf("Placing Object to (%d, %d) \n", setX, setY);
    MapObject** newnodes[sizeX*sizeY];
    for (int x=0; x<sizeX; x++){
        for (int y=0; y<sizeY; y++){
            newnodes[x+y] = getMapNode(setX+x, setY+y);

            // Teste ob Objekt dort platziert werden darf.
            if((*newnodes[x+y]) != nullptr){
                printf("ERROR: Zielposition bereits belegt! \n");
                return false;
            }
        }
    }

    for (int i=0; i< sizeX*sizeY; i++){
        *newnodes[i]=this;
    }

    return true;
}

void Object::unsetPos(){
    for (int x=0; x<sizeX; x++){
        for (int y=0; y<sizeY; y++){
            *getMapNode(posX+x, posY+y)=nullptr;
        }
    }
}

bool MapObject::teleport(int setX, int setY){
    bool success = setPos(setX, setY);
    if(success){ 
        unsetPos();
        posX=setX;
        posY=setY;
    }

    return success;
}

bool Object::moveRight(){
    sprite.setScale(texture->scaleX * (-1), texture->scaleY); // look right
    return teleport(posX+1, posY);
}
bool Object::moveLeft(){
    sprite.setScale(texture->scaleX, texture->scaleY); // look left
    return teleport(posX-1, posY);
}
bool Object::moveDown(){
    return teleport(posX, posY+1);
}
bool Object::moveUp(){
    return teleport(posX, posY-1);
}
*/