#ifndef O_LIVING
#define O_LIVING
#include "Object.hpp"
#include "Itempanel.hpp"

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// LAYER 1
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

class LivingObject : public Object {
protected:
    Texsheet* tex_moving;
    bool killed = false;
    // Animation:
    struct movementAnimation {
        uint state; // reihe in Texsheet
        uint frames = 0; // also animationsiterationen
        //uint blockCycles = 0; // block Update-cycles
        sf::Time time;
        sf::Vector2i endPos;
        sf::Vector2f move;
    } movementAnimation;
    struct specialAnimation {
        Texsheet* tex;
        uint frames=0;
        sf::Time time, last, passed;
    }specialAnimation;


////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////// 
public:
    LivingObject(Object ** map, int x, int y, Texsheet* tex=nullptr, sf::Vector2i d = DOWN) : 
        Object(map, x, y, tex, d)
    {
        identity.insert(LIVING);
    };

    bool wasKilled(){return killed;}


    // nur genau einen Schritt moven
    virtual uint step(sf::Vector2i _dir, uint factor=1){
         // vorerst über teleport() -> Bei größeren objekten (also größer als 1x1) ist es aber sinnvoller, nicht alle Felder jedes Mal neu zu belegen, sondern immer nur das erste Feld in die zu bewegende richtung.
        if(dir != _dir){
            // erstmal nur in die gewünschte Richtung schauen. Noch kein Schritt.
            setDirection(_dir);
            return 0;
        }

        sf::Vector2i startpos = pos;
        for(uint i=1; i <= factor; i++){
            if(!setMapPosition(pos + _dir)){
                // Living Obj. ist wogegen gelaufen. -> interact.
                std::cout << "interaction throgh touching " << std::endl;
                interact(this);
                std::cout << "interaction throgh touching done" << std::endl;
                return i-1;
            }

            
            movementAnimation.frames = 5*factor;
            movementAnimation.time = sf::milliseconds(18);
            movementAnimation.state = dtm[{dir.x, dir.y}];
            movementAnimation.endPos = sf::Vector2i(pos.x * OBJECTUNIT, pos.y * OBJECTUNIT);

            sf::Vector2i deltaPos = pos - startpos;
            movementAnimation.move = sf::Vector2f( 
                ((float)deltaPos.x * (float)OBJECTUNIT) / (float)movementAnimation.frames, 
                ((float)deltaPos.y * (float)OBJECTUNIT) / (float)movementAnimation.frames);
        }
        return factor;
    };

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
    public: virtual bool animate(sf::Time available) override{
        // FKT bekommt insgesamt noch für die ganze AnimationQueue zur Abarbeitung zur Verfügung stehende Zeit

        if(!animated)return false;

        if(movementAnimation.frames > 0){
            standingAnimation.passed = sf::milliseconds(0);
            return animateMovement(available); 
        }else if(specialAnimation.frames > 0){
            standingAnimation.passed = sf::milliseconds(0);
            return animateSpecial(available);
        }
        else return animateStanding(available);
    };

    bool animateMovement(sf::Time available){
        if(movementAnimation.frames <= 0) return false;
        sf::Time timePerStep = available / (float)movementAnimation.frames;
        if(timePerStep > movementAnimation.time){
            return false;
        }
        sprite.setTexture(tex_moving->texture);
        movementAnimation.frames--;

        sf::Vector2i texsize = (sf::Vector2i) tex_moving->getSize();
        uint curFrame = (movementAnimation.frames) % tex_moving->getImageCount().x;
        sf::Vector2i position(texsize.x * curFrame, texsize.y * movementAnimation.state);
        sprite.setTextureRect(sf::IntRect(position,texsize));

        

        std::cout << "Shifting to\tR: " << movementAnimation.state << "\tC: "<< curFrame << "\tRemaining: "<< movementAnimation.frames << "\tMove: "<< movementAnimation.move.x << ", " << movementAnimation.move.y << std::endl;

        sprite.move(movementAnimation.move);

        if(movementAnimation.frames <= 0){
                // setze endposition, falls floatingpointrechnungen unten böse numerik gemacht haben
                sprite.setPosition((sf::Vector2f)movementAnimation.endPos);
        }

        return true;

    }

    ///////////////////

    bool animateSpecial(sf::Time available){
        if(available > specialAnimation.last){
            specialAnimation.passed += specialAnimation.last + available - UPDATE_TIME;
        }else{
            specialAnimation.passed += specialAnimation.last - available;
        } 
        specialAnimation.last = available;

        if(specialAnimation.passed < specialAnimation.time){
            return false;
        }
        specialAnimation.passed = sf::milliseconds(0);

        specialAnimation.frames--;

        std::cout << "Remaining: "<< specialAnimation.frames << std::endl;

        if(specialAnimation.tex != nullptr){

            sprite.setTexture(specialAnimation.tex->texture);
            sf::Vector2i texsize = (sf::Vector2i) specialAnimation.tex->getSize();
            uint imgcount = specialAnimation.tex->getImageCount().x;

            uint curFrame = imgcount -((specialAnimation.frames) % imgcount) -1;
            sf::Vector2i position(texsize.x * curFrame, texsize.y * dtm[{dir.x, dir.y}]);
            sprite.setTextureRect(sf::IntRect(position,texsize));
        }

        return true;
    }

    ////////////////////////////////////////////////////////////////////////////////

};





#endif //O_LIVING
