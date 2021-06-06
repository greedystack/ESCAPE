#ifndef LEVEL_H
#define LEVEL_H

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics.hpp>
#include "Tex.h"
#include "Object.hpp"

// Stellt ein Level dar und verwaltet die Map etc.

class Level {
public:
    // OLD
    
    Tex **textures;
    sf::Font font;
    Tex*  getTexture(char c, int i){return textures[(uint16_t)c+i];};
    void loadTexture(char c, int i, Tex* t){textures[(uint16_t)c+i] = t;};
    void loadTextures();
    void loadFonts();

    //////////////////////////////////////////////////////////////////////////////////////////////
    
    Level(uint64_t x, uint64_t y)
        : mapsizex(x), mapsizey(y)
    {
        map = (Object**)(malloc((2 + x * y) * sizeof(Object *)));

        for(int i=0; i<(x*y); i++){
            map[i] = nullptr;
        }

        map[0] = (Object*)x;
        map[1] = (Object*)y;

        printf("Map allocated \n");

        // OLD:
        textures = (Tex**)(malloc(5 * sizeof(uint8_t) * sizeof(Tex *)));
        loadTextures();
        loadFonts();
        ////

        
        place(new LivingObject(map, 10, 10, getTexture('w', 0), LEFT));

        place(new Item(map, 10, 3, getTexture('w', 0)));
        

        Object* obj = getObject(sf::Vector2i(10,3));
        obj->getInteracted();

        player = new Player(map, 7, 4, getTexture('w', 0));
        place(player);
        
        /*
        new Barrier(map, 0, 0, getTexture('w', 0));
        new Barrier(map, 0, 1, getTexture('w', 0));
        new Barrier(map, 0, 2, getTexture('w', 0));
        new Barrier(map, 1, 2, getTexture('w', 0));
        new Barrier(map, 2, 2, getTexture('w', 0));
        new Barrier(map, 3, 3, getTexture('w', 0));
        new Barrier(map, 4, 3, getTexture('w', 0));
        new Barrier(map, 5, 3, getTexture('w', 0));
        new Barrier(map, 6, 3, getTexture('w', 0));
        */
        //buildBorders();
    }

    ~Level(){
        for(int i = 0; i < mapsizex*mapsizey; i++){
            if(&map[i] != nullptr){
                free(&map[i]);
            }
        }
        free(map);
        map = nullptr;
        printf("Map freed \n");
    }
    
    //////////////////////////////////////////////////////////////////////////////////////////////

    Player* getPlayer(){return player;};
    int getMapX(){return mapsizex;};
    int getMapY(){return mapsizey;};



    Object** getNode(sf::Vector2<int> position){
        return &map[position.x * mapsizey + position.y +2];
    }
    Object* getObject(sf::Vector2<int> position){
        return *getNode(position);
    }


    bool place(Object* obj){
        printf("Placing Object to (%d, %d) \n", obj->pos.x, obj->pos.y);
    
        if(! isFree(obj->pos, obj->size)){
            // TODO: Dann woanders platzieren?
            std::cout << "ERROR: Position bereits belegt." << std::endl;
        }

        for (int x=0; x < obj->size.x; x++){
            for (int y=0; y < obj->size.y; y++){
                *getNode(obj->pos + sf::Vector2i(x, y)) = obj;
            }
        }
    };

    // Prüft, ob eine (Ziel-)position auf Map frei ist
    bool isFree(sf::Vector2<int> check_pos, sf::Vector2<int> offset, Object* allowed = nullptr){
        for (int x=0; x < offset.x; x++){
            for (int y=0; y < offset.y; y++){
                Object* obj = getObject(check_pos + sf::Vector2i(x, y));
                if(obj != nullptr && obj != allowed){
                    return false;
                }
            }
        }
        return true;
    };

    bool interact(Object* obj){
        if(isFree((obj->pos + obj->dir), obj->size)) return false;
        Object* iteractee = getObject(obj->pos + obj->dir);
        std::cout << "Interactee: (" << iteractee->pos.x << ", " << iteractee->pos.y << ")" << std::endl;
        iteractee->getInteracted();
    }

//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////

private:
    const int OBJECTUNIT = 20; // ACHTUNG AUCH NOCHMAL IM GAME FÜR DEN RENDERER DEFINIERT!
    Object** map;
    Player* player;
    int mapsizex, mapsizey;

    void buildBorders();
    void drawBackground(int);
};



#endif //LEVEL_H