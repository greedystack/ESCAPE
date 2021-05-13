//
// Created by Marius on x
//

#include "../MapObject.h"

#ifndef FIRST_MAP_H
#define FIRST_MAP_H



class Map {
    public:
        Map(int x=0, int y=0);
        void mmalloc(int, int);
        int getX(){return cX;};
        int getY(){return cY;};
        bool move(short);
        MapObject *movableObj= nullptr;
        MapObject *staticObj= nullptr;

        static Map* root;

        static Map* getFieldStatic(int, int, Map*);
        static Map* getFieldRoot(int x, int y){return getFieldStatic(x, y, root);}
        Map* getField(int x, int y){return getFieldStatic(x, y, this);}

        static MapObject player[4];
        static Map* createPlayer(Tex*, short, int, int);

        void listNodes(int, int);

    private:
        int cX, cY; // coordinates
        Map *left, *up, *right, *down;

        bool collisionDetect(int, int);

};



#endif //FIRST_MAP_H
