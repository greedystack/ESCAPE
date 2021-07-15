#ifndef O_ENEMY
#define O_ENEMY
#include "O_Living.hpp"

class Enemy : public LivingObject {
public:
    Enemy(Object ** map, int x, int y) : 
        LivingObject(map, x, y, texsheets["minotaurus_standing"], DOWN)
    {
        identity.insert(ENEMY);
        tex_moving = texsheets["minotaurus_move"];
        movementAnimation.time= sf::milliseconds(13);
    };

    virtual bool interact(Object* interactee=nullptr) override{
        if(interactee->whoami().contains(PLAYER)){
            // Töte Player
            sf::Vector2i playerDir = interactee->pos - pos;
            //setDirection(playerDir);

            enqueueSpecialAnimation(texsheets["minotaurus_kill"], 7, 30, playerDir);
            enqueueSpecialAnimation(texsheets["minotaurus_kill"], 7, 40, playerDir, true);
            enqueueSpecialAnimation(texsheets["minotaurus_kill"], 7, 30, playerDir);
        }
    };

    virtual bool getInteracted(Object* interacter=nullptr) override{
        if(interacter->whoami().contains(PLAYER)){
            // Wurde vom Player getötet.
            sf::Vector2i playerDir = interacter->pos - pos;
            //setDirection(playerDir);
            enqueueSpecialAnimation(texsheets["minotaurus_killed"], 14, 50, playerDir);
            killed = true;
        }
    };

    virtual uint step(sf::Vector2i _dir, uint factor=1) override {
        uint steps = LivingObject::step(_dir, factor);
        movementAnimation.frames=8;
        for(auto d : {RIGHT, UP, LEFT, DOWN}){
            if(neighbor(d) == nullptr) continue;
            if(neighbor(d)->whoami().contains(PLAYER)){
                //setDirection(d);
                neighbor(d)->getInteracted(this);
            }
        }
        return steps;
    }

    virtual void update() override{
        if(saq.size() < 1){
            if(killed){
                del();
            }
            else{
                /*
                // Do a step ... maybe.
                uint r = rand() % 8;
                if(r == 0){
                    sf::Vector2i _dir;
                    r = rand() % 2;
                    if(r == 0){
                        // choose direction
                        r = rand() % 4;
                        std::array directions = {LEFT, UP, RIGHT, DOWN};
                        _dir = directions[r];
                    } else {
                        // use same direction
                        _dir = dir;
                    }
                    step(_dir);
                }
                */
            }
        }
    }

    
    
    //virtual void kill() override {del();}

};

#endif //O_ENEMY