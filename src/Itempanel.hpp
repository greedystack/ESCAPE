
#ifndef PANELOBJECT
#define PANELOBJECT

#include"Texsheet.hpp"

extern sf::Font STANDARDFONT;

const sf::Color status_bordercolor(50, 50, 50, 255);
const sf::Color status_bgcolor(100, 100, 100, 225);
const sf::Color status_bgcolorselected(255, 50, 50, 225);

const sf::Color status_bgcolornotselectable(120, 120, 120, 150);
const sf::Color status_bordercolornotselectable(75, 75, 75, 255);

const float radius = 60;

const float outlinethickness = 5;
const sf::Vector2f abstand(25, 30);
const float imagewidth = 90;

const sf::Color amount_bgcolor(45, 45, 45, 180);
const float radius_amount = 18;
const sf::Vector2f offset_amountbubble(70, 70);
const float charactersize = 22;

const float charactersize_inUse = 52;
const float imagewidth_inUse = 30;
const sf::Color status_bgcolorinUse(0, 175, 0, 235);
const sf::Color status_bordercolorInUse(70, 70, 70, 235);
const sf::Color maxamountcolor = status_bgcolorselected;

class Panelobject{
protected:
    sf::Vector2f pos;
    Texsheet* tex;
    sf::CircleShape circle, amountcircle;
    sf::Sprite img;
    sf::Text text;
    int steps, amount, maxAmount, stepsAvailable;
    bool selectable;

    void updateText(int num){
        std::string amnt = "99+";
        if(num <= 99){
            amnt = std::to_string(num);
        }else if(num < 0){
            amnt = "0";
        }
        text.setString(amnt);
        sf::FloatRect textRect = text.getLocalBounds();
        text.setOrigin(textRect.left + textRect.width/2.0f,
               textRect.top  + textRect.height/2.0f);
    }
    void setInuse(){
        circle.setFillColor(status_bgcolorinUse);
        circle.setOutlineColor(status_bordercolorInUse);
        circle.setOutlineThickness(outlinethickness);

        text.setFont(STANDARDFONT);
        text.setCharacterSize(charactersize_inUse);
        text.setFillColor(sf::Color::White);
        text.setStyle(sf::Text::Bold);
        text.setPosition(pos + sf::Vector2f(radius, radius));
        updateText(steps);
        
        img.setColor(sf::Color(200,200,200,150));
        /*
        float scalefactor = (float) (imagewidth_inUse) / tex->getSize().x;
        img.setTexture(tex->texture);
        //img.setPosition(pos.x + radius + (offset_amountbubble.x - (imagewidth_inUse)/2), 
        //        pos.y + radius + ( offset_amountbubble.y - ((tex->getSize().y * scalefactor)/2)));
        img.setPosition(pos + offset_amountbubble);
        img.setScale(sf::Vector2f(scalefactor, scalefactor));
        */
        
    }
    void setUnused(){
        circle.setFillColor(status_bgcolor);
        circle.setOutlineColor(status_bordercolor);
        circle.setOutlineThickness(outlinethickness);

        text.setFont(STANDARDFONT);
        text.setCharacterSize(charactersize);
        text.setFillColor(sf::Color::White);
        text.setStyle(sf::Text::Bold);
        text.setPosition(pos + offset_amountbubble + sf::Vector2f(radius_amount, radius_amount));
        updateText(amount);

        img.setColor(sf::Color(255,255,255,255));
        float scalefactor = (float) (imagewidth) / tex->getSize().x;
        img.setTexture(tex->texture);
        img.setPosition(pos.x + (radius - (imagewidth)/2), 
                pos.y + ( radius - ((tex->getSize().y * scalefactor)/2)));
        img.setScale(sf::Vector2f(scalefactor, scalefactor));
    }
    void setUnselectable(){
        circle.setFillColor(status_bgcolornotselectable);
        circle.setOutlineColor(status_bordercolornotselectable);
    }

public:

    Panelobject(sf::Vector2f _pos = sf::Vector2f(0,0), 
                Texsheet* _tex = nullptr, bool _selectable=true, int _stepsAvailable = 25, int _maxAmount = 0)
        : pos(_pos), tex(_tex), stepsAvailable(_stepsAvailable), maxAmount(_maxAmount), selectable(_selectable)
    {
        amount=0;
        steps=0;

        if(tex != nullptr){
            circle.setRadius(radius);
            circle.setPosition(pos);

            amountcircle.setRadius(radius_amount);
            amountcircle.setFillColor(amount_bgcolor);
            amountcircle.setPosition(pos + offset_amountbubble);

            setUnused();
            if(!selectable)setUnselectable();
        }
    };



    void draw(sf::RenderWindow& w){
        w.draw(circle);
        if(steps <= 0){
            w.draw(img);
            w.draw(amountcircle);
            w.draw(text);
        } else {
            //w.draw(amountcircle);
            w.draw(img);
            w.draw(text);
            
        }
    }

    void select(){
        circle.setFillColor(status_bgcolorselected);
    }
    void unselect(){
        circle.setFillColor(status_bgcolor);
    }


    bool use(int _amount=1){
        if(amount < _amount) return false;
        if(steps >0) return false;
        amount -= _amount;
        if(selectable){
            steps += stepsAvailable;
            unselect();
            setInuse();
        }else{
            updateText(amount);
            amountcircle.setFillColor(amount_bgcolor);
        }
        return true;
    }
    void step(uint s){
        steps-=s;
        updateText(steps);
        if(steps <= 0){
            steps = 0;
            setUnused();
        }
    }
    bool add(int _amount=1){
        if(amount + _amount <= maxAmount || maxAmount <= 0){
            amount += _amount;
            updateText(amount);
            if(amount == maxAmount){
                amountcircle.setFillColor(maxamountcolor);
            }
            return true;
        } 
        return false;
    }
    int getamount(){return amount;}
    int getsteps(){return steps;}
    bool isselectable(){return selectable && steps<=0 && amount > 0;}
};

#endif //PANELOBJECT

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef ITEMPANEL
#define ITEMPANEL

class Itempanel {
protected:
    std::map<int, Panelobject> panel;
    std::map<int, Panelobject>::iterator it;
public:
    static const int NAVI, MARKER, FOOD;
    Itempanel(int t){
        sf::Vector2f pos = abstand;
        panel[Itempanel::NAVI] = Panelobject(pos, Object::texsheets["navi"], true, 25);
        pos += sf::Vector2f(0, 2*radius + abstand.y);
        panel[Itempanel::MARKER] = Panelobject(pos, Object::texsheets["bread"], true, 25);
        pos += sf::Vector2f(0, 2*radius + abstand.y);
        panel[Itempanel::FOOD] = Panelobject(pos, Object::texsheets["bambus"], false, 0, 10);
        pos += sf::Vector2f(0, 2*radius + abstand.y);

        it = panel.begin();
    }

    void draw(sf::RenderWindow& w){
        for(auto p : panel){
            p.second.draw(w);
        }
    }

    bool enableSelection(){
        int start = it->first;
        while(!it->second.isselectable()){
            it++;
            if(it == panel.end()) it = panel.begin();
            if(it->first == start) return false;
        }
        it->second.select();
        return true;
    }
    void disableSelection(){
        it->second.unselect();
    }
    void up(){
        it->second.unselect();
        do{
            if(it == panel.begin()) it = panel.end();
            it--;
        }while(!it->second.isselectable());
        it->second.select();
    }
    void down(){
        it->second.unselect();
        do{
            it++;
            if(it == panel.end()) it = panel.begin();
        }while(!it->second.isselectable());
        it->second.select();
    }

    bool use(int type=0, int amount=1){
        if(type == 0) type = it->first;
        return panel[type].use(amount);
    }

    bool add(int type, int amount = 1){
        return panel[type].add(amount);
    }

    void step(uint steps){
        
        for(auto p : panel){
            if(getSteps(p.first))
                panel[p.first].step(steps);
        }
    }

    int getSteps(int type){
        return panel[type].getsteps();
    }
    int getAmount(int type){
        return panel[type].getamount();
    }


};

const int Itempanel::NAVI = 1;
const int Itempanel::MARKER = 2;
const int Itempanel::FOOD = 3;

#endif //ITEMPANEL