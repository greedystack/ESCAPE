//
// Created by Marius on 02.01.20.
//

#include "MapObject.h"


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