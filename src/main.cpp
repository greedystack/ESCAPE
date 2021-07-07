//#include <iostream>

#include <string>
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include "Level.hpp"
#include <unistd.h>
#include <iostream>



// in ObjectUnit:
const int BGSIZE=100; // minimum half of screensize!! // must be divideable by 2


const std::string TITLE = "Escape!";

const uint OBJECTUNIT = 20; // Pixel pro Map-Block
const sf::Time MAX_updateTime = sf::milliseconds(10); // fastest possible move speed
const sf::Time STD_updateTime = sf::milliseconds(45); // regular move speed
const sf::Time DELTA_updateTime = sf::milliseconds(2); // änderungsvektor für beschleunigung

sf::Time UPDATE_TIME = STD_updateTime;
sf::Vector2u WIN_SIZE(1024, 1024); // in Pixel
float zoom = 0.5f; // inverted (also im Sinne von Kehrwert)

int lvl_count = 2;

int main()
{
    sf::RenderWindow window(sf::VideoMode(WIN_SIZE.x, WIN_SIZE.y), TITLE);
    sf::View view;

    sf::Clock clock, animationclock;
    sf::Time elapsed = clock.restart();

    sf::Vector2u start(0,0), end(0,0); // Mapblocks zum Rendern
    
    // keep track if we have to redraw things. No need if nothing has been updated!
    bool updateView = true;
    bool paused = false;
    bool drawAnimation = false;
    bool interactionBlocked = false;
    int speedup = 0;

    Level* level = new Level(2000, 2000);
    

    while (window.isOpen())
    {
        // add the time passed since the last cycle
        elapsed = elapsed + clock.restart(); // This function puts the time counter back to zero. It also returns the time elapsed since the clock was started.

        ////////////////////////////////////////////////
        ///// Events
        ////////////////////////////////////////////////
        sf::Event event;
        while (window.pollEvent(event))
        {
            switch (event.type)
            {
                case sf::Event::Closed:
                    window.close();
                    break;
                case sf::Event::GainedFocus:
                    std::cout << "gained focus" << std::endl;
                    paused = false;
                    break;
                case sf::Event::LostFocus:
                    std::cout << "lost focus" << std::endl;
                    paused = true;
                    break;
                case sf::Event::Resized:
                    std::cout << "new width: " << event.size.width << std::endl;
                    std::cout << "new height: " << event.size.height << std::endl;
                    WIN_SIZE = sf::Vector2u(event.size.width, event.size.height);
                    updateView = true;
                    break;
                default:
                    //std::cout << "-UNKNOWN EVENT: " << event.type << std::endl;
                    break;
            }
        }
        

        if(paused){
            usleep(250000); // Heizung aus! ;)
            continue;
        } 
        ////////////////////////////////////////////////
        ///// Eingabe
        ////////////////////////////////////////////////
        // make as many updates as needed for the elapsed time
        if(!interactionBlocked){
            while (elapsed > UPDATE_TIME)
            {   
                speedup-=1;
                if(sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
                {
                    level->getPlayer()->step(LEFT);
                    updateView = true;
                    speedup+=2;
                }
                if(sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
                {
                    level->getPlayer()->step(UP);
                    updateView = true;
                    speedup+=2;
                }
                if(sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
                {
                    level->getPlayer()->step(RIGHT);
                    updateView = true;
                    speedup+=2;
                }
                if(sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
                {
                    level->getPlayer()->step(DOWN);
                    updateView = true;
                    speedup+=2;
                }
                if(sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
                {
                    // Interact with Object before Player
                    level->getPlayer()->interact();
                    updateView = true;
                }
                if(sf::Keyboard::isKeyPressed(sf::Keyboard::Return) 
                    || sf::Keyboard::isKeyPressed(sf::Keyboard::Enter))
                {
                    // Item Bag
                    level->getPlayer()->useItem(); // temporarily
                    updateView = true;
                }
                if(sf::Keyboard::isKeyPressed(sf::Keyboard::Add) ||
                    sf::Keyboard::isKeyPressed(sf::Keyboard::M))
                {
                    zoom-=0.05;
                    updateView = true;
                }
                if(sf::Keyboard::isKeyPressed(sf::Keyboard::Subtract) ||
                    sf::Keyboard::isKeyPressed(sf::Keyboard::N))
                {
                    zoom+=0.05;
                    updateView=true;
                }

                elapsed -= UPDATE_TIME;
            }
        }

        ////////////////////////////////////////////////
        ///// Speedup
        ////////////////////////////////////////////////

        if(speedup<0){
            // keine Bewegung stattgefunden
            speedup=0;
            UPDATE_TIME = STD_updateTime; // Stoppe schnelles Laufen.
        }else if(speedup>3){
            // Bewegung in `speedup-1` aufeinanderfolgenden Loops stattgefunden.
            speedup=0;
            if(UPDATE_TIME >= MAX_updateTime){
                UPDATE_TIME -= DELTA_updateTime; // Beschleunige
            }
        }

        ////////////////////////////////////////////////
        ///// Animate
        ////////////////////////////////////////////////
        interactionBlocked = false;
        for (uint x = start.x; x < end.x; x++){
            for (uint y = start.y; y < end.y; y++){
                Object* m = level->getObject(sf::Vector2u(x, y));
                if (m == nullptr) continue;
                if(!m->animated) continue;
                m->passedTime += animationclock.getElapsedTime();
                if(m->passedTime < m->switchTime) continue;
                m->passedTime = sf::milliseconds(0);
                if(!m->update()){
                    interactionBlocked = true;
                }
                updateView=true;

            }
        }
        animationclock.restart();

        ////////////////////////////////////////////////
        //////////////// Update Window
        ////////////////////////////////////////////////
        if(updateView || drawAnimation){
            if(updateView){
                //std::cout<< "Beschleunigung: " << updateTime.asSeconds() << "\n";

                ////////////////////////////////////////////////
                //////////////// Level Won
                ////////////////////////////////////////////////
                if(level->getPlayer()->hasWon()){
                    lvl_count--;
                    if(lvl_count <= 0){
                        // WON GAME
                        std::cout << "HURRA! HURRA!" << std::endl;
                        // TODO Win Animation
                    }else{
                        std::cout << "init new level" << std::endl;
                        delete level;
                        level = new Level(2000, 2000);
                    }
                }
                
                ////////////////////////////////////////////////
                //////////////// Calculate View Size and View
                ////////////////////////////////////////////////
                sf::Vector2f playerpos = level->getPlayer()->sprite.getPosition();
                float xfrom, yfrom;

                // x
                if(playerpos.x < WIN_SIZE.x *zoom *0.5){
                    // Player ist so weit links, dass er nicht mehr zentriert angezeigt werden kann.
                    xfrom=0;
                }else if(playerpos.x > level->getMapX()*OBJECTUNIT - WIN_SIZE.x *zoom *0.5){
                    // Player ist so weit rechts, dass er nicht mehr zentriert angezeigt werden kann.
                    xfrom = level->getMapX()*OBJECTUNIT - WIN_SIZE.x *zoom;
                }else{
                    // Player ist x-mittig im view
                    xfrom = playerpos.x - WIN_SIZE.x *zoom *0.5;
                }

                // y
                if(playerpos.y < WIN_SIZE.y *zoom *0.5){
                    // Player ist so weit oben, dass er nicht mehr zentriert angezeigt werden kann.
                    yfrom=0;
                }else if(playerpos.y > level->getMapY()*OBJECTUNIT - WIN_SIZE.y *zoom *0.5){
                    // Player ist so weit unten, dass er nicht mehr zentriert angezeigt werden kann.
                    yfrom = level->getMapY()*OBJECTUNIT - WIN_SIZE.y *zoom;
                }
                else{
                    // Player ist x-mittig im view
                    yfrom = playerpos.y - WIN_SIZE.y *zoom *0.5;
                }


                view.reset(sf::FloatRect(xfrom, yfrom, WIN_SIZE.x*zoom, WIN_SIZE.y*zoom ));

                // Adjust Render from-to
                start = sf::Vector2u(
                    xfrom/OBJECTUNIT,
                    yfrom/OBJECTUNIT
                );
                end= sf::Vector2u(
                    (xfrom + WIN_SIZE.x*zoom)/OBJECTUNIT,
                    (yfrom + WIN_SIZE.x*zoom)/OBJECTUNIT
                );
                //std::cout << "Drawing: ("<< start.x <<", "<< start.y<<") ";
                //std::cout << "to ("<< end.x <<", "<< end.y<<")\n";
            }


            ////////////////////////////////////////////////
            //////////////// Draw Window
            ////////////////////////////////////////////////
            // Clear screen
            window.clear(sf::Color::White);
            window.setView(view);


            
            window.draw(level->getBackground());

            // Render only Objects inside View
            for (uint x = start.x; x < end.x; x++){
                for (uint y = start.y; y < end.y; y++){
                    Object* m = level->getObject(sf::Vector2u(x, y));
                    if (m == nullptr) continue;
                    while(m != nullptr){// falls ich Stacking implementiere.
                        //if (!m->visible) continue;
                        //if(x != m->posX || y != m->posY ) continue; // For Objects bigger than 1x1 map-field

                        window.draw(m->sprite);
                        //m=m->onTop; // falls ich Stacking implementiere.
                        m=nullptr; // falls nicht
                    }
                }
            } 

            

            // Draw the string
            //window.draw(text);


            // Update the window
            window.display();
            updateView = false;
        }
        ////////////////////////////////////////////////
    }

    delete level;

    return EXIT_SUCCESS;
}
