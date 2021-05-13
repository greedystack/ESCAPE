//
// Created by Marius on x
//

#include "Map.h"

Tex defaultTex = Tex("../include/img/mario.png", 1, 1);

Map* Map::root = nullptr;
MapObject Map::player[4] = {MapObject(&defaultTex, 4,2), MapObject(&defaultTex, 4,2), MapObject(&defaultTex, 4,2), MapObject(&defaultTex, 4,2)};

Map::Map(int x, int y)
: cX(x), cY(y)
{
    if(Map::root == nullptr) Map::root = this;
    printf("new node: %d, %d\n", cX, cY);
}

// Map Malloc
void Map::mmalloc(int sizeX, int sizeY){
    Map *top = this;
    for (int x = 0; x < sizeX; x++) {
        top->right = new Map(x, 0); // neuer Top-Block
        top->right->left = top;
        top = top->right;
        Map *bttm = top;

        for (int y = 1; y < sizeY; y++) { // erste Zeile immer auslassen
            bttm->down = new Map(x, y); // Unten block anfügen
            bttm->down->up = bttm; //diesen nach oben verlinken

            if(x>0) bttm->down->left = bttm->left->down; // wenn nicht erste spalte, mit linker spalte verlinken
        }

    }
}

Map* Map::createPlayer(Tex* tex, short id, int x, int y){
    player[id] = MapObject(tex, 4, 8, x, y);
    Map* field = getFieldRoot(x, y);
    field->movableObj = &player[0];
    return field;
}

// TODO: 2. Konstruktor für txt Mapimport :)
// TODO: 3. Konstruktor für random Map

bool Map::move(short dir){
    if(movableObj == NULL) return false;
    Map* go_to;
    switch(dir) {
        case 1: //left
            go_to=left;
            break;
        case 2: //up
            go_to=up;
            break;
        case 3: //right
            go_to=right;
            break;
        case 4: //down
            go_to=down;
            break;
        default:
            break;
    }
    printf("goto: %d, %d", go_to->getX(), go_to->getY());
    printf("dir: %d", dir);
    if (go_to->movableObj != nullptr || go_to->staticObj != nullptr) {
        go_to = this;
        dir = 0;
    }
    printf("dir: %d", dir);
    if (movableObj->move(dir)){
        printf("goto");
        go_to->movableObj=movableObj;
        movableObj=NULL;
        return true;
    }
    return false;
}

Map* Map::getFieldStatic(int x, int y, Map* start){
    Map *map = start;
    while(map->cX < x){
        map = map->right;
    }
    while(map->cX > x){
        map = map->left;
    }
    while(map->cY < y){
        map = map->down;
    }
    while(map->cY > y){
        map = map->up;
    }
    return map;
}

void Map::listNodes(int xx, int yy){
    Map* top=this;
    printf("Root-Node (%d, %d)\t l: (%d, %d)\t t: (%d, %d)\t r: (%d, %d)\t b: (%d, %d)", 0, 0,
           top->left->getX(), top->left->getY(),
           top->up->getX(), top->up->getY(),
           top->right->getX(), top->right->getY(),
           top->down->getX(), top->down->getY());
    for (int x=0; x<xx; x++){
        top = top->right;
        Map *bttm = top->down;
        printf("Top-Node (%d, %d)\t l: (%d, %d)\t t: (%d, %d)\t r: (%d, %d)\t b: (%d, %d)", x, 0,
               top->left->getX(), top->left->getY(),
               top->up->getX(), top->up->getY(),
               top->right->getX(), top->right->getY(),
               top->down->getX(), top->down->getY());
        for (int y=1; y<yy; y++){
            printf("Node (%d, %d)\t l: (%d, %d)\t t: (%d, %d)\t r: (%d, %d)\t b: (%d, %d)", x, y,
                   bttm->left->getX(), bttm->left->getY(),
                   bttm->up->getX(), bttm->up->getY(),
                   bttm->right->getX(), bttm->right->getY(),
                   bttm->down->getX(), bttm->down->getY());
            bttm=bttm->down;
        }
    }
}