//#include <iostream>

#include <string>
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include "Level.hpp"
#include <unistd.h>
#include <iostream>




const std::string TITLE = "Escape!";

const uint OBJECTUNIT = 20; // Pixel pro Map-Block
const sf::Time UPDATE_TIME = sf::milliseconds(99);

sf::Vector2u WIN_SIZE(2000, 2000); // in Pixel
float zoom = 0.5f; // inverted (also im Sinne von Kehrwert)
// const sf::Vector2u MAX_VIEW_SIZE(); // in Mapblocks - um keinen vorteil durch rauszommen oder resizen zu bekommen

int lvl_count = 3;

int main()
{
    sf::RenderWindow window(sf::VideoMode(WIN_SIZE.x, WIN_SIZE.y), TITLE);
    sf::View view;

    sf::Clock clock;
    sf::Time elapsed = clock.restart();

    sf::Vector2u start(0,0), end(0,0); // Mapblocks zum Rendern
    
    // keep track if we have to redraw things. No need if nothing has been updated!
    bool updateView = true;
    bool paused = false;
    bool drawAnimation = false;

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
        
        // make as many updates as needed for the elapsed time
        while (elapsed > UPDATE_TIME)
        {   
            if(!level->getPlayer()->isParalyzed()){
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
                //////////////// Killed. Restart level
                ////////////////////////////////////////////////
                if(level->getPlayer()->wasKilled()){
                    level->reset();
                }


                ////////////////////////////////////////////////
                ///// User Input
                ////////////////////////////////////////////////
            
                uint speed = 1;
                if(sf::Keyboard::isKeyPressed(sf::Keyboard::Period))speed++;
                if(sf::Keyboard::isKeyPressed(sf::Keyboard::Comma))speed++;
                if(sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
                {
                    level->getPlayer()->step(LEFT, speed);
                    updateView = true;
                }
                if(sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
                {
                    level->getPlayer()->step(UP, speed);
                    updateView = true;
                }
                if(sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
                {
                    level->getPlayer()->step(RIGHT, speed);
                    updateView = true;
                }
                if(sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
                {
                    level->getPlayer()->step(DOWN, speed);
                    updateView = true;
                }
                if(sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
                {
                    // Interact with Object before Player
                    //level->getPlayer()->interact();
                    //updateView = true;
                }
                if(sf::Keyboard::isKeyPressed(sf::Keyboard::Return) 
                    || sf::Keyboard::isKeyPressed(sf::Keyboard::Enter))
                {
                    // Item Bag
                    level->getPlayer()->useItem(); // temporarily
                    updateView = true;
                }
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

            ////////////////////////////////////////////////
            ///// Update (z.B. Enemies bewegen)
            ////////////////////////////////////////////////
            for (uint x = start.x; x < end.x; x++){
                for (uint y = start.y; y < end.y; y++){
                    Object* m = level->getObject(sf::Vector2u(x, y));
                    if (m == nullptr) continue;
                    m->update();
                }
            }


            elapsed -= UPDATE_TIME;
        }


        ////////////////////////////////////////////////
        ///// Animate
        ////////////////////////////////////////////////
        for (uint x = start.x; x < end.x; x++){
            for (uint y = start.y; y < end.y; y++){
                Object* m = level->getObject(sf::Vector2u(x, y));
                if (m == nullptr) continue;
                // Idee: Hier Zeit mitgeben, wie lang insgesamt noch für alle Animationen zur Verfügung steht.
                if(m->animate(UPDATE_TIME - elapsed)){
                    updateView=true;
                }
                
            }
        }

        


        

        
        

        ////////////////////////////////////////////////
        //////////////// Update Window
        ////////////////////////////////////////////////
        if(updateView || drawAnimation){
            if(updateView){
                //std::cout<< "Beschleunigung: " << updateTime.asSeconds() << "\n";

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
                    ((xfrom + WIN_SIZE.x*zoom)/OBJECTUNIT)+1,
                    ((yfrom + WIN_SIZE.y*zoom)/OBJECTUNIT)+1
                );
                //std::cout << "Drawing: ("<< start.x <<", "<< start.y<<") ";
                //std::cout << "to ("<< end.x <<", "<< end.y<<")\n";
                /*
                std::cout << 
                "Zoom*Pixel:\tx: " << zoom*WIN_SIZE.x << 
                "\ty: " << zoom*WIN_SIZE.y << 
                "\tPixel_x: " << WIN_SIZE.x << 
                "\tPixel_y: " << WIN_SIZE.y << 
                "\tzoom: " << zoom << std::endl;*/
            }


            ////////////////////////////////////////////////
            //////////////// Draw Window
            ////////////////////////////////////////////////
            // Clear screen
            window.clear(sf::Color::White);
            window.setView(view);


            
            window.draw(level->getBackground());
            if(level->getPlayer()->getNaviSteps() > 0){
                Object nav = level->getPlayer()->getNavi(
                    level->getDirection(level->getPlayer()->pos.x, level->getPlayer()->pos.y)
                );
                window.draw(nav.sprite);
            }


            // Render only Objects inside View
            for (uint x = start.x; x < end.x; x++){
                for (uint y = start.y; y < end.y; y++){
                    Object* m = level->getObject(sf::Vector2u(x, y));
                    if (m == nullptr || m == level->getPlayer()) continue;
                    
                    while(m != nullptr){// falls ich Stacking implementiere.

                        //if (!m->visible) continue;
                        //if(x != m->posX || y != m->posY ) continue; // For Objects bigger than 1x1 map-field
    
                        window.draw(m->sprite);
                        //m=m->onTop; // falls ich Stacking implementiere.
                        m=nullptr; // falls nicht
                    }
                    
                }
            }
            window.draw(level->getPlayer()->sprite);
            

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
