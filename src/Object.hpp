#ifndef OBJECT_H
#define OBJECT_H

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include "Texsheet.hpp"
#include <iostream>
#include <list>
#include <set>
#include <map>
#include <queue>

// Ein Objekt auf der Map. IDEE: Könnte von Sprite erben.
// Problem: Da draw() von Sprite private... 

const sf::Vector2i RIGHT(1,0);
const sf::Vector2i LEFT(-1,0);
const sf::Vector2i UP(0,-1);
const sf::Vector2i DOWN(0,1);
    

const uint ANIMATION_STEPS_PER_FIELD = 10;


// Identifier for whoami()
const uint OBJECT = 0;
const uint ITEM = 1;
const uint BARRIER = 2;
const uint PORTAL = 3;
const uint GOAL = 31;
const uint DESTROYER = 11;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// LAYER 0
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
class Object {
////////////////////////////////////////////////////////////////////////////////
///// STATIC
////////////////////////////////////////////////////////////////////////////////
public:
    static std::map<std::string, Texsheet*> texsheets;

    static void loadTexsheets(std::string theme = "standard"){
        texsheets["bg0"] = new Texsheet("../include/img/boden2.png");
        texsheets["hecke"] = new Texsheet("../include/img/hecke.png", 1, 16);
        texsheets["wall"] = new Texsheet("../include/img/wall0.png");
        texsheets["arrow_left"] = new Texsheet("../include/img/arrow.png", 1, 4);
        texsheets["panda"] = new Texsheet("../include/img/panda.png", 3, 8);
    }
////////////////////////////////////////////////////////////////////////////////

public:
    sf::Sprite sprite;
    sf::Vector2i pos, size, dir; // Position ist immer oben links am Objekt. Size ist dessen Größe in Map-Nodes.
    bool animated=false, visible=false;

    // Animation:
    sf::Time passedTime, switchTimeRegular = sf::milliseconds(1000), switchTime = switchTimeRegular;
    
protected:
    Texsheet *tex;
    Object** map;
    int OBJECTUNIT = 20;
    std::set<uint> identity;

    // Animation:
    uint frame;
    struct ani {
        uint state; // reihe in Texsheet
        uint frames; // also animationsiterationen
        sf::Time time;
        sf::Vector2u endPos;
        sf::Vector2f move;
    } ;
    std::queue<ani> animationQueue;
    ani standardAnimation;
    ani movementAnimation;

    std::map<std::array<int, 2>, uint> dtm; //direction texsheet map

public:
    ////////////////////////////////////////////////////////////////////////////////
    //Object* r, l, u, d, on; // Ich bau den Müll jetzt in eine 2d-Liste um, wenn ich eh schon bei identisch großen Objekten bleibe.
    //Object** connected=nullptr; // das wäre dann ein Array für größere Objekte - die bestehen dann aus einelnen subobjekten.
    ////////////////////////////////////////////////////////////////////////////////

    Object(Object** m, int x, int y, Texsheet* t = nullptr, sf::Vector2i d=LEFT, int sx=1, int sy=1) 
        : map(m), pos(x, y), size(sx, sy), tex(t)
    {
        identity.insert(OBJECT);
        if (tex != nullptr){
            visible=true;
            sprite.setTexture(tex->texture);
            setScale();
            setDirection(d);
            
        }
        place();
        sprite.setPosition((float)pos.x*OBJECTUNIT, (float)pos.y*OBJECTUNIT);

        if(tex->getImageCount().x > 1) animated=true;


        dtm[ {LEFT.x, LEFT.y} ] = 0; 
        dtm[{UP.x, UP.y}] = 1; 
        dtm[{RIGHT.x, RIGHT.y}] = 2; 
        dtm[{DOWN.x, DOWN.y}] = 3;
    };

    std::set<uint> whoami(){return identity;}

    ////////////////////////////////////////////////////////////////////////////////

    virtual bool getInteracted(Object* interacter){ // passive interaktion
        return false;
    };
    virtual bool interact(Object* interactee){ //aktive interaktion
        return false;
    };

    virtual bool update(){ // Fkt. für Animationen und so
    // returned false, wenn animation noch nicht fertig - also weiterer input gesperrt werden muss.
        return true;
    }
    
    ////////////////////////////////////////////////////////////////////////////////
    // Map-Functions

    int getMapsizeX(){
        return (uint64_t) map[0];
    }
    int getMapsizeY(){
        return (uint64_t) map[1];
    }

    Object** getNode(sf::Vector2<int> position){
        if(!isInBounds(position)) return nullptr;
        return &map[position.x * getMapsizeY() + position.y +2];
    }
    Object* getObject(sf::Vector2<int> position){
        if(!isInBounds(position)) return nullptr;
        return *getNode(position);
    }

    bool isInBounds(sf::Vector2i position){
        if(position.x >= getMapsizeX() 
            || position.y >= getMapsizeY()
            || position.x <0 
            || position.y < 0
        ){
            //std::cout<< "ERROR: Position (" << position.x <<", "<< position.y <<") out of Array!\n";
            return false;
        }
        return true;
    }

    // ACHTUNG!! Nur für 1x1-große Objekte gebaut! 
    Object* neighbor(sf::Vector2i _dir){
        return getObject(pos+_dir);
    };

    // ACHTUNG!! Nur für 1x1-große Objekte gebaut! 
    // Entfernt Objekt restlos.
    void del(){
        removeFromMap();
        delete this;
    };

protected:
    bool place(){
        //printf("Placing Object to (%d, %d) \n", pos.x, pos.y);
    
        if(! isFree(pos, size)){
            // TODO: Dann vielleicht woanders platzieren?
            //std::cout << "ERROR: Position bereits belegt." << std::endl;
            return false;
        }

        for (int x=0; x < size.x; x++){
            for (int y=0; y < size.y; y++){
                *getNode(pos + sf::Vector2i(x, y)) = this;
            }
        }
        return true;
    };

    

    // ACHTUNG!! Nur für 1x1-große Objekte gebaut! 
    // ACHTUNG!! Entfernt Objekt nur von Map, nicht aus dem Speicher! Also nu sinnvoll, wenn Pointer dann woanders gespeichert wird (Item Bag zB)
    void removeFromMap(){
        *getNode(pos) = nullptr;
        pos = sf::Vector2i(-1, -1);
    };
    
    // ACHTUNG!! Nur für 1x1-große Objekte gebaut! 
    void putOnMap(sf::Vector2i _pos){
        *getNode(_pos) = this;
        pos = _pos;
    };

    // Prüft, ob eine (Ziel-)position auf Map frei ist
    bool isFree(sf::Vector2i check_pos, sf::Vector2i offset){
        for (int x=0; x < offset.x; x++){
            for (int y=0; y < offset.y; y++){
                Object* obj = getObject(check_pos + sf::Vector2i(x, y));
                if(obj != nullptr && obj != this){
                    return false;
                }
            }
        }
        return true;
    };

    // Objekte "teleportieren"
    bool teleport(sf::Vector2<int> set_pos){
        if(!setMapPosition(set_pos)){
            return false;
        }

        sprite.setPosition((float)pos.x*OBJECTUNIT, (float)pos.y*OBJECTUNIT);
        return true;
    };

    bool setMapPosition(sf::Vector2<int> set_pos){
        // map-ränder verkleben
        if(set_pos.x < 0) set_pos.x = getMapsizeX() +set_pos.x;
        else if(set_pos.x >= getMapsizeX()) set_pos.x = set_pos.x - getMapsizeX();
        if(set_pos.y < 0) set_pos.y = getMapsizeY() +set_pos.y;
        else if(set_pos.y >= getMapsizeY()) set_pos.y = set_pos.y - getMapsizeY();

        //printf("Moving Object to (%d, %d) \n", set_pos.x, set_pos.y);
        
        if(! isFree(set_pos, size)){
            //std::cout << "ERROR: Position bereits belegt." << std::endl;
            return false;
        } 

        // Befreie alte Pos.
        for (int x=0; x < size.x; x++){
            for (int y=0; y < size.y; y++){
                *getNode(pos + sf::Vector2i(x, y)) = nullptr;
            }
        }

        // Setze neue Pos.
        for (int x=0; x < size.x; x++){
            for (int y=0; y < size.y; y++){
                *getNode(set_pos + sf::Vector2i(x, y)) = this;
            }
        }
        
        pos.x = set_pos.x;
        pos.y = set_pos.y;

        return true;
    };

////////////////////////////////////////////////////////////////////////////////
/// Optik
////////////////////////////////////////////////////////////////////////////////

    public:
    sf::Time getRefreshTime(){
        if(animationQueue.size() > 0) return  animationQueue.front().time;
        return standardAnimation.time;
    }

    bool refresh(sf::Time elapsed){
        if(!animated) return false;

        passedTime += elapsed;
        if(passedTime < getRefreshTime()) return false;

        //iterateAnimation();
        passedTime -= getRefreshTime();

        return true;
    }

    

    bool animate(sf::Time available){
        // FKT bekommt insgesamt noch für die ganze AnimationQueue zur Abarbeitung zur Verfügung stehende Zeit

        if(!animated)return false;

        ani* a;
        if(movementAnimation.frames <= 0) return animateStanding(available);
        else return animateMovement(available);
    };

    bool animateStanding(sf::Time available){
        return false;
    }

    bool animateMovement(sf::Time available){
        sf::Time timePerStep = available / (float)movementAnimation.frames;
        if(timePerStep > movementAnimation.time){
            return false;
        }

        movementAnimation.frames--;

        sf::Vector2i texsize = (sf::Vector2i) tex->getSize();
        uint curFrame = (movementAnimation.frames)%3; // TODO @HARDCODED
        sf::Vector2i position(texsize.x * curFrame, texsize.y * movementAnimation.state);
        sprite.setTextureRect(sf::IntRect(position,texsize));

        

        std::cout << "Shifting to\tR: " << movementAnimation.state << "\tC: "<< curFrame << "\tRemaining: "<< movementAnimation.frames << "\tQ: "<< animationQueue.size() << "\tMove: "<< movementAnimation.move.x << ", " << movementAnimation.move.y << std::endl;

        sprite.move(movementAnimation.move);

        if(movementAnimation.frames <= 0){
                // setze endposition, falls floatingpointrechnungen unten böse numerik gemacht haben
                std::cout << "SET: " << movementAnimation.endPos.x << ", " << movementAnimation.endPos.y << std::endl;
                sprite.setPosition((sf::Vector2f)movementAnimation.endPos);
        }

        return true;

    }


    ////////////////////////////////////////////////////////////////////////////////
    ////// Nochmal überdenken:
    
    protected:
    void setTexType(uint row){
        if(tex->getImageCount().y <= row){
            std::cout << "ERROR: Texsheet hat nicht so viele Modi. Setze Modus 0.";
            row = 0;
        }
        standardAnimation = createAnimation(row, 0, switchTimeRegular);

        sf::Vector2i texsize = (sf::Vector2i) tex->getSize();
        sf::Vector2i position(0, texsize.y * row);
        sprite.setTextureRect(sf::IntRect(position,texsize));
        // std::cout << "Texsize: " << texsize.x << ", " << texsize.y * row << std::endl;
    };

    /*
    void iterateAnimation2(){
        if(!animated){
            return;
        }

        frame++;
        if(frame >= tex->getImageCount().x){
            frame = 0;
        }

        ani* cur;
        if(animationQueue.size() <= 0){
            cur = &standardAnimation;
            cur->frames++;

        }else{
            cur = &animationQueue.front();
            if(cur->frames <= 1){
                if(cur->move != sf::Vector2i(0,0)){
                    // setze endposition, falls floatingpointrechnungen unten böse numerik gemacht haben
                    std::cout << "SET: " << cur->endPos.x << ", " << cur->endPos.y << std::endl;
                    sprite.setPosition((sf::Vector2f)cur->endPos);
                }
                animationQueue.pop();
            }
            cur->frames--;
        }

        
        
        sf::Vector2i texsize = (sf::Vector2i) tex->getSize();
        uint curFrame = (cur->frames)%3;
        sf::Vector2i position(texsize.x * curFrame, texsize.y * cur->state);
        sprite.setTextureRect(sf::IntRect(position,texsize));

        

        std::cout << "Shifting to\tR: " << cur->state << "\tC: "<< curFrame << "\tRemaining: "<< cur->frames << "\tQ: "<< animationQueue.size() << "\tMove: "<< cur->move.x << ", " << cur->move.y << std::endl;

        //sprite.setPosition(sprite.getPosition() + (sf::Vector2f)cur->move);
        switchTime = cur->time;
    };
    */


    ani createAnimation(uint row, uint frames, sf::Time time, sf::Vector2i startPos=sf::Vector2i(0,0), sf::Vector2i endPos=sf::Vector2i(0,0)){ 
        // Adds Animation to queue
        ani a;

        a.endPos = sf::Vector2u(endPos.x*OBJECTUNIT, endPos.y*OBJECTUNIT);
        
        a.move=sf::Vector2f(0.,0.);
        if(startPos != endPos){
            sf::Vector2i deltaPos = (sf::Vector2i)endPos - (sf::Vector2i)startPos;
            a.move = sf::Vector2f(
                (float)(deltaPos.x * OBJECTUNIT) / frames, 
                (float)(deltaPos.y * OBJECTUNIT) / frames);
        }
        
        a.state = row;
        a.frames = frames;
        a.time = time;
        return a;
    }

    // update texture direction gemäß vorliegendem wert -> wird überschrieben zB bei Objekten mit mehr als 4 richtungen (die blöden Hecken)
    public:
    virtual void updateDirection(){
        setTexType(dtm[{dir.x, dir.y}]);
    }

    protected:
    // Change direction to provided direction and change texture
    void setDirection(sf::Vector2i _dir){
        if(dir == _dir) return;
        dir = _dir;
        updateDirection();
    };

    void setScale(){
        sf::Vector2f scale(
            (float) (OBJECTUNIT * size.x) / tex->getSize().x,
            (float) (OBJECTUNIT * size.y) / tex->getSize().y);
        sprite.scale(scale);
    }

};
////////////////////////////////////////////////////////////////////////////////
///// Static vars ankündigen:
////////////////////////////////////////////////////////////////////////////////
std::map<std::string, Texsheet*> Object::texsheets;

#endif //OBJECT_H
