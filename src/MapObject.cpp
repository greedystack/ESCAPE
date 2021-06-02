//
// Created by Marius on 02.01.20.
//

#include "MapObject.h"

MapObject::MapObject(MapObject** m, Tex* tex, char t, int x, int y, int size_x, int size_y)
    : map(m), texture(tex), sizeX(size_x), sizeY(size_y), type(t), posX(x), posY(y){
        if (map != nullptr) {
            if(!setPos(posX, posY)){
                // TODO ! alternative pos finden -> besser als exception throwen
                printf("ERROR! Konnte Objekt bei erstellung nicht Positionieren, da Position besereits belegt!");
            }
            
            if (texture != nullptr){
                visible=true;
                sprite.setTexture(texture->regular);
                sprite.setOrigin(0.5, 0.5);
                //sprite.setRotation((float) rotation*90);
            }
        }
}

////////////////////////////////////////////////////////////////////////////////


bool MapObject::setPos(int setX, int setY){
    printf("Placing Object to (%d, %d) \n", setX, setY);
    MapObject** newnodes[sizeX*sizeY];
    for (int x=0; x<sizeX; x++){
        for (int y=0; y<sizeY; y++){
            newnodes[x+y] = getMapNode(setX+x, setY+y);

            // Teste ob Objekt dort platziert werden darf.
            if((*newnodes[x+y]) != nullptr){
                printf("ERROR: Bewegung nicht erlaubt! \n");
                return false;
            }
        }
    }

    for (int i=0; i< sizeX*sizeY; i++){
        *newnodes[i]=this;
    }

    return true;
}

void MapObject::unsetPos(){
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

bool MapObject::moveRight(){
    sprite.setScale(texture->scaleX * (-1), texture->scaleY); // look right
    return teleport(posX+1, posY);
}
bool MapObject::moveLeft(){
    sprite.setScale(texture->scaleX, texture->scaleY); // look left
    return teleport(posX-1, posY);
}
bool MapObject::moveDown(){
    return teleport(posX, posY+1);
}
bool MapObject::moveUp(){
    return teleport(posX, posY-1);
}


////////////////////////////////////////////////////////////////////////////////
// Alt
// Schaue in Map an Koordinate (ClassFunctions)
MapObject** MapObject::getMapNode(int x, int y){
    return &map[x * (uint64_t) map[1] + y + (uint64_t) map[0]];
}
MapObject* MapObject::getMapObject(int x, int y){
    return *getMapNode(x, y);
}

////////////////////////////////////////////////////////////////////////////////
