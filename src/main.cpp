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


const std::string TITLE = "Best game ever!";

const uint OBJECTUNIT = 20; // Pixel pro Map-Block
const sf::Time MAX_updateTime = sf::milliseconds(10); // fastest possible move speed
const sf::Time STD_updateTime = sf::milliseconds(45); // regular move speed
const sf::Time DELTA_updateTime = sf::milliseconds(2); // änderungsvektor für beschleunigung

sf::Time updateTime = STD_updateTime;
sf::Vector2u WIN_SIZE(1024, 1024); // in Pixel
float zoom = 0.5f; // inverted

int main()
{
    sf::RenderWindow window(sf::VideoMode(WIN_SIZE.x, WIN_SIZE.y), TITLE);
    sf::View view;

    sf::Clock clock;
    sf::Time elapsed = clock.restart();
    
    // keep track if we have to redraw things. No need if nothing has been updated!
    bool update = true;
    bool paused = false;
    int speedup = 0;

    Level level(2000, 2000);
    

    while (window.isOpen())
    {
        // add the time passed since the last cycle
        elapsed = elapsed + clock.restart();

        
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
                    update = true;
                    break;
                default:
                    //std::cout << "-UNKNOWN EVENT: " << event.type << std::endl;
                    break;
            }
        }
        

        if(paused) continue;
        
        // make as many updates as needed for the elapsed time
        while (elapsed > updateTime)
        {   
            speedup-=1;
            if(sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
            {
                level.getPlayer()->step(LEFT);
                update = true;
                speedup+=2;
            }
            if(sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
            {
                level.getPlayer()->step(UP);
                update = true;
                speedup+=2;
            }
            if(sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
            {
                level.getPlayer()->step(RIGHT);
                update = true;
                speedup+=2;
            }
            if(sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
            {
                level.getPlayer()->step(DOWN);
                update = true;
                speedup+=2;
            }
            if(sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
            {
                // Interact with Object before Player
                level.getPlayer()->interact();
                update = true;
            }
            if(sf::Keyboard::isKeyPressed(sf::Keyboard::Return) 
                || sf::Keyboard::isKeyPressed(sf::Keyboard::Enter))
            {
                // Item Bag
                level.getPlayer()->useItem(); // temporarily
                update = true;
            }
            if(sf::Keyboard::isKeyPressed(sf::Keyboard::Add) ||
                sf::Keyboard::isKeyPressed(sf::Keyboard::M))
            {
                zoom-=0.05;
                update = true;
            }
            if(sf::Keyboard::isKeyPressed(sf::Keyboard::Subtract) ||
                sf::Keyboard::isKeyPressed(sf::Keyboard::N))
            {
                zoom+=0.05;
                update=true;
            }
            /*
            if(sf::Keyboard::isKeyPressed(sf::Keyboard::A))
            { // TEST
                sf::Vector2f scale = level.getPlayer()->sprite.getScale();
                if(scale.x >= 0.f){
                    level.getPlayer()->sprite.setScale(scale.x *(-1.f), scale.y);
                    level.getPlayer()->sprite.move(OBJECTUNIT, 0);
                }else{
                    level.getPlayer()->sprite.setScale(scale.x *(-1.f), scale.y);
                    level.getPlayer()->sprite.move((-1.f)*OBJECTUNIT, 0);
                }
                std::cout << level.getPlayer()->sprite.getRotation() << std::endl;
                update=true;
            }
            */


            // don't forget to subtract the updateTime each cycle ;-)
            elapsed -= updateTime;
        }

        if(speedup<0){
            // keine Bewegung stattgefunden
            speedup=0;
            updateTime = STD_updateTime; // Stoppe schnelles Laufen.
        }else if(speedup>3){
            // Bewegung in `speedup-1` aufeinanderfolgenden Loops stattgefunden.
            speedup=0;
            if(updateTime >= MAX_updateTime){
                updateTime -= DELTA_updateTime; // Beschleunige
            }
        }
        
        if(update){
            //std::cout<< "Beschleunigung: " << updateTime.asSeconds() << "\n";
            //Set View
            if(level.getPlayer()->hasWon()){
                // TODO Level löschen, neues Level erstellen
                std::cout << "HURRA! HURRA!" << std::endl;
            }
            
            sf::Vector2f playerpos = level.getPlayer()->sprite.getPosition();
            float xfrom, yfrom;

            // x
            if(playerpos.x < WIN_SIZE.x *zoom *0.5){
                // Player ist so weit links, dass er nicht mehr zentriert angezeigt werden kann.
                xfrom=0;
            }else if(playerpos.x > level.getMapX()*OBJECTUNIT - WIN_SIZE.x *zoom *0.5){
                // Player ist so weit rechts, dass er nicht mehr zentriert angezeigt werden kann.
                xfrom = level.getMapX()*OBJECTUNIT - WIN_SIZE.x *zoom;
            }else{
                // Player ist x-mittig im view
                xfrom = playerpos.x - WIN_SIZE.x *zoom *0.5;
            }

            // y
            if(playerpos.y < WIN_SIZE.y *zoom *0.5){
                // Player ist so weit oben, dass er nicht mehr zentriert angezeigt werden kann.
                yfrom=0;
            }else if(playerpos.y > level.getMapY()*OBJECTUNIT - WIN_SIZE.y *zoom *0.5){
                // Player ist so weit unten, dass er nicht mehr zentriert angezeigt werden kann.
                yfrom = level.getMapY()*OBJECTUNIT - WIN_SIZE.y *zoom;
            }
            else{
                // Player ist x-mittig im view
                yfrom = playerpos.y - WIN_SIZE.y *zoom *0.5;
            }


            view.reset(sf::FloatRect(xfrom, yfrom, WIN_SIZE.x*zoom, WIN_SIZE.y*zoom ));

            // Clear screen
            window.clear(sf::Color::White);
            window.setView(view);


            // Render Objects inside View
            sf::Vector2u start = sf::Vector2u(
                xfrom/OBJECTUNIT,
                yfrom/OBJECTUNIT
            );
            sf::Vector2u end = sf::Vector2u(
                (xfrom + WIN_SIZE.x*zoom)/OBJECTUNIT,
                (yfrom + WIN_SIZE.x*zoom)/OBJECTUNIT
            );

            std::cout << "Drawing: ("<< start.x <<", "<< start.y<<") ";
            std::cout << "to ("<< end.x <<", "<< end.y<<")\n";

            for (uint x = start.x; x < end.x; x++){
                for (uint y = start.y; y < end.y; y++){
                    Object* m = level.getObject(sf::Vector2u(x, y));
                    if (m == nullptr) continue;
                    //printf("RENDERING (%d, %d)\n", x, y);
                    //if(x != m->posX || y != m->posY ) continue; // For Objects bigger than 1x1 map-field
                    //if (!m->visible) continue;
                    window.draw(m->sprite);
                }
            } 

            

            // Draw the string
            //window.draw(text);


            // Update the window
            window.display();
            update = false;
        }
    }

    return EXIT_SUCCESS;
}
