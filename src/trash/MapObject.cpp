//
// Created by Marius on 02.01.20.
//

#include "MapObject.h"

MapObject::MapObject(MapObject** m, Tex* tex, char t, int x, int y, int size_x, int size_y)
    : map(m), texture(tex), sizeX(size_x), sizeY(size_y), type(t), posX(x), posY(y){
        if (map != nullptr) {
            MapObject **n = getMapNode(posX, posY);
            if (*n == nullptr){
                printf("creating first Pointer on node (%d, %d) %x \n", posX, posY, n);
                *n=this;
            }else {
                MapObject *m = *n;
                printf("Allocating Node (%d, %d) %x on top of Node %x ... \n", posX, posY, this, m);
                while (m->onMe != nullptr) {
                    printf("looking in subnode %x \n", m->onMe);
                    m = m->onMe;
                }
                printf("placing behind last subnode %x \n", m);
                m->onMe = this;
            }
            onMe=nullptr;
            // no passable-check when constructing. Maybe useful?
            if (texture != nullptr){
                visible=true;
                sprite.setTexture(texture->regular);
                sprite.setOrigin(0.5, 0.5);
                //sprite.setRotation((float) rotation*90);
            }


        }
}

////////////////////////////////////////////////////////////////////////////////

bool MapObject::move(signed int dir, int setX, int setY) {
    int posXnew = posX, posYnew = posY;

    

    // todo in int[][] umwandeln:
    MapObject** oldnodes[sizeX*sizeY];
    MapObject** newnodes[sizeX*sizeY];
    switch(dir) {
        case -1: // teleport
            for (int x=0; x<sizeX; x++)
                for (int y=0; y<sizeY; y++){
                    oldnodes[x+y]=getMapNode(posX+x, posY+y);
                    newnodes[x + y] = getMapNode(setX+x, setY+y);
                }
            break;
        case 1: //left
            posXnew--;
            for (int x=0; x<sizeX; x++)
                for (int y=0; y<sizeY; y++){
                    oldnodes[x+y]=getMapNode(posX+x, posY+y);
                    newnodes[x + y] = getMapNode(posX+x-1, posY+y);
                }
            sprite.setScale(texture->scaleX, texture->scaleY); // look left
            break;
        case 2: //up
            posYnew--;
            for (int y=0; y<sizeY; y++)
                for (int x=0; x<sizeX; x++){
                    oldnodes[x+y]=getMapNode(posX+x, posY+y);
                    newnodes[x + y] = getMapNode(posX+x, posY+y-1);
                }
            break;
        case 3: //right
            posXnew++;
            for (int x=sizeX-1; x>=0; x--)
                for (int y=0; y<sizeY; y++){
                    oldnodes[x+y]=getMapNode(posX+x, posY+y);
                    newnodes[x + y] = getMapNode(posX+x+1, posY+y);
            }

            sprite.setScale(texture->scaleX * (-1), texture->scaleY); // look right
            break;
        case 4: //down
            posYnew++;
            for (int y=sizeY; y>=0; y--)
                for (int x=0; x<sizeX; x++) {
                    oldnodes[x + y] = getMapNode(posX + y, posY + y);
                    newnodes[x + y] = getMapNode(posX+x, posY+y+1);
                }
            break;
        default: // no move
            break;
    }

    if(onMe != nullptr) printf("on player: %x\n", onMe);

    // TODO: Check passable for all! Dann erst MapNodes

    for (int i=0; i<sizeX*sizeY; i++){
        bool success = true;
        if(*newnodes[i] != nullptr) success = (*newnodes[i])->placeOnMe(this);
        else *newnodes[i]=this;
        if(success){
            (*oldnodes[i])->removeFromMe(this);
            posX=posXnew, posY=posYnew;
        }
    }
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

// Platziere ein Objekt auf dem aktuellen (stacking)
bool MapObject::placeOnMe(MapObject* obj){
    printf("Setting top Object (%d, %d) on bottom \t%x\n", posX, posY, this);
    MapObject* m=this;
    while (m->onMe != nullptr){
        m = m->onMe;
        printf("%x,  ", m);
    }
    printf("\n");
    if(!m->isPassable()) return false;
    printf("Set right behind %x in queue\n", m);
    m->onMe = obj;
    return true;
}

bool MapObject::removeFromMe(MapObject* obj){;
    if (this == obj) {
        printf("Remove bottom object (%d, %d)\t%x\n\n", posX, posY, obj);
        *getMapNode(posX, posY) = nullptr;
        return true;
    }
    MapObject* m=this;
    printf("Remove stacked object (%d, %d)\t%x\n", posX, posY, obj);
    printf("Starting at Bottom %x\n", m);
    while (m->onMe != obj){
        if(m->onMe) return false;
        m = m->onMe;
        printf("%x,\t", m->onMe);
    }
    //m->onMe = m->onMe->onMe;
    printf("\n");
    m->onMe = obj->onMe;
    obj->onMe;
    return true;
}

////////////////////////////////////////////////////////////////////////////////