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
    struct MovementAnimationStruct {
        uint state; // reihe in Texsheet
        uint frames = 0; // also animationsiterationen
        //uint blockCycles = 0; // block Update-cycles
        sf::Time time;
        sf::Vector2i endPos;
        sf::Vector2f move;
    } movementAnimation;
    struct SpecialAnimationStruct {
        Texsheet* tex;
        uint frames=0;
        sf::Vector2i dir;
        sf::Time time, last, passed;
        bool reverse;
    }specialAnimation;

    std::queue<SpecialAnimationStruct> saq;

    void enqueueSpecialAnimation(Texsheet* _tex, uint _frames, uint ms, sf::Vector2i _dir = sf::Vector2i(0,0), bool _reverse = false){
        if(_dir == sf::Vector2i(0,0)) _dir = dir;
        SpecialAnimationStruct ani;
        ani.tex = _tex;
        ani.frames = _frames;
        ani.time = sf::milliseconds(ms);
        ani.dir = _dir;
        ani.reverse = _reverse;
        saq.push(ani);
    }


////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////// 
public:
    LivingObject(Object** m, int x, int y, Texsheet* t=nullptr, sf::Vector2i d = DOWN) : 
        Object(m, x, y, t, d)
    {
        identity.insert(LIVING);
    };

    bool wasKilled(){return killed;}


    uint getSAQduration(){
        uint t = 0;
        std::queue<SpecialAnimationStruct> copy = saq;
        while(copy.size() > 0){
            t += copy.front().frames * copy.front().time.asMilliseconds();
            copy.pop();
        }
        return t;
    }


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
        }else if(saq.size() > 0){
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
        if(available > saq.front().last){
            saq.front().passed += saq.front().last + available - UPDATE_TIME;
        }else{
            saq.front().passed += saq.front().last - available;
        } 
        saq.front().last = available;

        if(saq.front().passed < saq.front().time){
            return false;
        }
        saq.front().passed = sf::milliseconds(0);

        saq.front().frames--;

        //std::cout << "Remaining: "<< saq.front().frames << std::endl;

        if(dir != saq.front().dir) dir = saq.front().dir;

        if(saq.front().tex != nullptr){

            sprite.setTexture(saq.front().tex->texture);
            sf::Vector2i texsize = (sf::Vector2i) saq.front().tex->getSize();
            uint imgcount = saq.front().tex->getImageCount().x;

            uint curFrame;
            if(saq.front().reverse)
                curFrame = (saq.front().frames) % imgcount;
            else
                curFrame = imgcount -((saq.front().frames) % imgcount) -1;

            sf::Vector2i position(texsize.x * curFrame, texsize.y * dtm[{dir.x, dir.y}]);
            sprite.setTextureRect(sf::IntRect(position,texsize));
        }

        if(saq.front().frames <= 0){
            saq.pop();
        }

        return true;
    }

    ////////////////////////////////////////////////////////////////////////////////

};





#endif //O_LIVING
