
#ifndef PANELOBJECT
#define PANELOBJECT

#include"Texsheet.hpp"

extern sf::Font STANDARDFONT;

const sf::Color status_bordercolor(50, 50, 50, 255);
const sf::Color status_bgcolor(100, 100, 100, 225);
const sf::Color status_bgcolorselected(255, 50, 50, 225);

const sf::Color status_bgcolorbambus(120, 120, 120, 150);

const float radius = 60;

const float outlinethickness = 5;
const sf::Vector2f abstand(25, 30);
const float imagewidth = 90;

const sf::Color amount_bgcolor(45, 45, 45, 180);
const float radius_amount = 18;
const sf::Vector2f offset_amountbubble(70, 70);
const float charactersize = 22;


class Panelobject{
protected:
    Texsheet* tex;
    sf::CircleShape circle, amountcircle;
    sf::Sprite img;
    sf::Text text;
    int steps, amount, maxAmount, stepsAvailable;

    void updateAmount(){
        std::string amnt = "99+";
        if(amount <= 99){
            amnt = std::to_string(amount);
        }
        text.setString(amnt);
        sf::FloatRect textRect = text.getLocalBounds();
        text.setOrigin(textRect.left + textRect.width/2.0f,
               textRect.top  + textRect.height/2.0f);
    }
public:
    static sf::Vector2f nextObject;

    Panelobject(Texsheet* _tex = nullptr, int _stepsAvailable = 25, int _maxAmount = 0)
        : tex(_tex), stepsAvailable(_stepsAvailable), maxAmount(_maxAmount)
    {
        amount=0;
        steps=0;

        if(tex != nullptr){
            circle.setRadius(radius);
            circle.setFillColor(status_bgcolor);
            circle.setOutlineColor(status_bordercolor);
            circle.setOutlineThickness(outlinethickness);
            circle.setPosition(nextObject.x, nextObject.y);

            amountcircle.setRadius(radius_amount);
            amountcircle.setFillColor(amount_bgcolor);
            amountcircle.setPosition(nextObject.x + offset_amountbubble.x, nextObject.y + offset_amountbubble.y);

            text.setFont(STANDARDFONT);
            text.setCharacterSize(charactersize);
            text.setFillColor(sf::Color::White);
            text.setStyle(sf::Text::Bold);
            text.setPosition(nextObject.x + offset_amountbubble.x + (radius_amount), 
                nextObject.y + offset_amountbubble.y + (radius_amount));
            updateAmount();

            float scalefactor = (float) (imagewidth) / tex->getSize().x;
            img.setTexture(tex->texture);
            img.setPosition(nextObject.x + (radius - (imagewidth)/2), 
                    nextObject.y + ( radius - ((tex->getSize().y * scalefactor)/2)));
            img.setScale(sf::Vector2f(scalefactor, scalefactor));

            nextObject += sf::Vector2f(0, 2*radius + abstand.y);
        }
    };

    

    void draw(sf::RenderWindow& w){
        w.draw(circle);
        w.draw(img);
        w.draw(amountcircle);
        w.draw(text);
    }

    void select(){
        circle.setFillColor(status_bgcolorselected);
    }
    void unselect(){
        circle.setFillColor(status_bgcolor);
    }


    bool use(int _amount=1){
        if(amount <= _amount) return false;
        amount -= _amount;
        steps += stepsAvailable;
        updateAmount();
        return true;
    }
    void step(uint _steps){
        if(steps <= 0) return;
        steps-=_steps;
        if(steps <= 0){

        }
    }
    bool add(int _amount=1){
        if(amount + _amount <= maxAmount || maxAmount <= 0){
            amount += _amount;
            updateAmount();
            return true;
        } 
        return false;
    }
    int getamount(){return amount;}
    int getsteps(){return steps;}
};

sf::Vector2f Panelobject::nextObject;

#endif //PANELOBJECT

///////////////////////////////////////

#ifndef ITEMPANEL
#define ITEMPANEL

class Itempanel {
protected:
    std::map<int, Panelobject> panel;
    std::map<int, Panelobject>::iterator it;
public:
    static const int NAVI, MARKER, FOOD;
    Itempanel(int t){
        Panelobject::nextObject = sf::Vector2f(abstand.x,abstand.y);
        panel[Itempanel::NAVI] = Panelobject(Object::texsheets["navi"], 25);
        panel[Itempanel::MARKER] = Panelobject(Object::texsheets["bread"], 25);
        panel[Itempanel::FOOD] = Panelobject(Object::texsheets["bambus"], 5);
        it = panel.begin();
    }

    void draw(sf::RenderWindow& w){
        for(auto p : panel){
            p.second.draw(w);
        }
    }

    void enableSelection(){
        it->second.select();
    }
    void disableSelection(){
        it->second.unselect();
    }
    void up(){
        it->second.unselect();
        it--;
        if(it == panel.end()) it--;
        it->second.select();
    }
    void down(){
        it->second.unselect();
        it++;
        if(it == panel.end()) it = panel.begin();
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
            p.second.step(steps);
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