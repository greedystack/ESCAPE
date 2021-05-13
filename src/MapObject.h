//
// Created by Marius on 02.01.20.
//

#ifndef FIRST_MAPOBJECT_H
#define FIRST_MAPOBJECT_H


#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include "Tex.h"


class MapObject {
public:
    sf::Sprite sprite;
    bool visible=false;

    MapObject(MapObject** m, Tex* tex, char t=' ', int x=0, int y=0, int size_x=1, int size_y=1)
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
    };

    int getX(){return posX;};
    int getY(){return posY;};
    bool isVisible(){return visible;};
    MapObject* getOnMe(){return onMe;};
    bool move(signed int, int=0, int=0);
    //bool setMovement(short[], MapObject**);
    void setColor(sf::Color c = sf::Color(0,0,0,0)){sprite.setColor(c);};

    bool isPassable(){
        if(type == 'P' || type == 'W') return false;
        return true;
    };
    bool isMovable(){
        if(type == 'P') return true;
        return false;
    };
    bool placeOnMe(MapObject* obj){
        printf("Setting top Object (%d, %d) in bottom \t%x\n", posX, posY, this);
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
    bool removeFromMe(MapObject* obj){;
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

    MapObject* getMapObject(int x, int y){return *getMapNode(x, y);};
    MapObject** getMapNode(int x, int y){return &map[x * (uint64_t) map[1] + y + (uint64_t) map[0]];};

protected:
    MapObject *onMe=nullptr, *connected, *allocatedTo;
private:
    char type;
    int posX, posY, sizeX, sizeY;
    MapObject** map;
    Tex* texture;
};

#endif //FIRST_MAPOBJECT_H
