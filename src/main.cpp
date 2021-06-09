//#include <iostream>

#include <string>
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include "Level.hpp"
#include <unistd.h>
#include <iostream>

const int OBJECTUNIT = 20; // Pixel pro Map-Block

// in ObjectUnit:
const int WINDOWSIZEX=100;
const int WINDOWSIZEY=100;
const int BGSIZE=100; // minimum half of screensize!! // must be divideable by 2

const sf::Time UPDATE_TIME = sf::milliseconds(30); // Move Speed

int main()
{
    sf::RenderWindow window(sf::VideoMode(WINDOWSIZEX*OBJECTUNIT, WINDOWSIZEY*OBJECTUNIT), "SFML window");
    sf::Clock clock;
    sf::Time elapsed = clock.restart();
    
    // keep track if we have to redraw things. No need if nothing has been updated!
    bool update = true;
    bool paused = false;

    Level level(1000, 1000);
    

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
                    //window.create());
                    // TODO: Window resizen!
                    break;
                default:
                    break;
            }
        }
        

        if(paused) continue;
        
        // make as many updates as needed for the elapsed time
        while (elapsed > UPDATE_TIME)
        {

            // classical key pressed checks
            if(sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
            {
                level.getPlayer()->step(LEFT);
                update = true;
            }
            if(sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
            {
                level.getPlayer()->step(UP);
                update = true;
            }
            if(sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
            {
                level.getPlayer()->step(RIGHT);
                update = true;
            }
            if(sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
            {
                level.getPlayer()->step(DOWN);
                update = true;
            }
            if(sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
            {
                level.getPlayer()->interact();
                update = true;
            }
            if(sf::Keyboard::isKeyPressed(sf::Keyboard::Return))
            {
                // Item Bag
                level.getPlayer()->useItem(); // temporarily
                update = true;
            }

            // don't forget to subtract the updateTime each cycle ;-)
            elapsed -= UPDATE_TIME;
        }

        
        if(update){
            // TODO: Views statt jeden Frame neu zu drawen?
            // Ist das wirklich effizient so? Gehen Views nicht auch?
            // https://www.sfml-dev.org/tutorials/2.2/graphics-view.php#showing-more-when-the-window-is-resized

            // Clear screen
            window.clear(sf::Color::White);

            int xfrom=level.getPlayer()->pos.x-(WINDOWSIZEX/2);
            int yfrom=level.getPlayer()->pos.y-(WINDOWSIZEY/2);
            if(xfrom < 0) xfrom = 0;
            if(yfrom < 0) yfrom = 0;
            if(xfrom+WINDOWSIZEX >= level.getMapX()) xfrom = level.getMapX() - WINDOWSIZEX;
            if(yfrom+WINDOWSIZEY >= level.getMapY()) yfrom = level.getMapY() - WINDOWSIZEY;

            //printf("refreshing window from (%d, %d) to (%d, %d)\n", xfrom, yfrom, xfrom+WINDOWSIZEX, yfrom+WINDOWSIZEY);

            for (int x = 0; x < WINDOWSIZEX; x++){
                for (int y = 0; y < WINDOWSIZEY; y++){
                    Object* m = level.getObject(sf::Vector2u(x+xfrom, y+yfrom));
                    if (m == nullptr) continue;
                    //if (typeid(*n) != typeid(VisibleObject)) continue;
                    //VisibleObject* m = (VisibleObject*)n;
                    //printf("RENDERING (%d, %d, %d) [%d]\n", x, y, z, m);


                    //if(x != m->posX || y != m->posY ) continue;
                    if (!m->visible) continue;

                    m->sprite.setPosition((float)x*OBJECTUNIT, (float)y*OBJECTUNIT);
                    window.draw(m->sprite);
                }
            } //*/


            // Draw the string
            //window.draw(text);


            // Update the window
            window.display();
            update = false;
        }
    }

    return EXIT_SUCCESS;
}
