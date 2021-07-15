#ifndef O_ENEMY
#define O_ENEMY
#include "O_Living.hpp"

class Enemy : public LivingObject {
public:
    Enemy(Object ** map, int x, int y) : 
        LivingObject(map, x, y, texsheets["minotaurus_standing"], UP)
    {
        identity.insert(ENEMY);
        tex_moving = texsheets["minotaurus_move"];
        movementAnimation.frames=8;
        movementAnimation.time= sf::milliseconds(13);
    };

    virtual bool interact(Object* interactee=nullptr) override{
        if(interactee->whoami().contains(PLAYER)){
            // Töte Player
            sf::Vector2i playerDir = interactee->pos - pos;
            setDirection(playerDir);
            
            specialAnimation.tex=texsheets["minotaurus_kill"];
            specialAnimation.frames=7;
            specialAnimation.time = sf::milliseconds(35);
        }
    };

    virtual bool getInteracted(Object* interacter=nullptr) override{
        if(interacter->whoami().contains(PLAYER)){
            // Wurde vom Player getötet.
            sf::Vector2i playerDir = interacter->pos - pos;
            setDirection(playerDir);

            specialAnimation.tex=texsheets["panda_killed"];
            specialAnimation.frames=19;
            specialAnimation.time = sf::milliseconds(50);
            killed = true;
        }
    };

    virtual uint step(sf::Vector2i _dir, uint factor=1) override {
        uint steps = LivingObject::step(_dir, factor);
        for(auto d : {RIGHT, UP, LEFT, DOWN}){
            if(neighbor(d) == nullptr) continue;
            if(neighbor(d)->whoami().contains(PLAYER)){
                setDirection(d);
                neighbor(d)->getInteracted(this);
            }
        }
        return steps;
    }

    virtual void update() override{
        if(specialAnimation.frames < 1){
            if(killed){
                del();
            }
            else{
                
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
                
            }
        }
    }

    
    
    //virtual void kill() override {del();}

};

#endif //O_ENEMY