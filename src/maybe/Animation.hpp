
#ifndef ANIMATION_HPP
#define ANIMATION_HPP


#include "Texsheet.hpp"
#include <string>

class Animation {
public:
    sf::IntRect rect;

private:
    Texsheet* texture;
    sf::Time switchTime; // Zeit zwischen frame-wechsel
    sf::Time passedTime; // interne Zeit
    uint row, framesLeft, fallbackState;
    sf::Vector2i move, endPos;
    Animation followUp;
    // state ist Animationsart (stehen, gehen, attacke... ) := Zeile in Spritesheet
    // frame ist Spalte in Spritesheet, also aktueller Frame der Animation.
    // state ändert sich von außen, frame ändert sich hier.





public:
    Animation(Texsheet* _texture, float _switchTime, uint _state=0, uint startframe=0)
        : texture(_texture), switchTime(_switchTime), state(_state), frame(startframe)
    {
        rect.width = texture->getSize().x;
        rect.height = texture->getSize().y;
        passedTime = 0.0f;
    };
    void changeState(uint newstate, uint cycles=1, uint fallback=0){
        state=newstate;
    };

    void update(float deltaTime){
        passedTime += deltaTime;

        if(passedTime >= switchTime){
            passedTime -= switchTime;
            frame++;
            if(frame >= texture->getImageCount().x){
                frame=0; // TODO >= hier richtig?

            } 
        }

        rect.left = frame * rect.width;
        rect.top = state * rect.height;
    };


};

#endif //ANIMATION_HPP
