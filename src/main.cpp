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

const uint OBJECTUNIT = 40; // Pixel pro Map-Block
const sf::Time UPDATE_TIME = sf::milliseconds(30); // Move Speed
sf::Vector2u WIN_SIZE(1024, 1024); // in Pixel

int main()
{
    sf::RenderWindow window(sf::VideoMode(WIN_SIZE.x, WIN_SIZE.y), TITLE);
    sf::View view;

    sf::Clock clock;
    sf::Time elapsed = clock.restart();
    
    // keep track if we have to redraw things. No need if nothing has been updated!
    bool update = true;
    bool paused = false;

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

            //Set View

            sf::Vector2f playerpos = level.getPlayer()->sprite.getPosition();
            //std::cout << "Player: ("<< playerpos.x << ", "<< playerpos.y << ")";
            //std::cout << " ("<< playerpos.x/OBJECTUNIT << ", "<< playerpos.y/OBJECTUNIT << ")\t";
            float xfrom = playerpos.x - WIN_SIZE.x *0.5;
            if(xfrom < 0){ 
                // Player ist so weit links, dass er nicht mehr zentriert angezeigt werden kann.
                xfrom=0;
            }else if(xfrom >= level.getMapX()*OBJECTUNIT - WIN_SIZE.x){
                // Player ist so weit unten, dass er nicht mehr zentriert angezeigt werden kann.
                xfrom = level.getMapX()*OBJECTUNIT - WIN_SIZE.x;
            }

            float yfrom = playerpos.y - WIN_SIZE.y *0.5;
            if(yfrom < 0){ 
                // Player ist so weit oben, dass er nicht mehr zentriert angezeigt werden kann.
                yfrom=0;
            }else if(yfrom >= level.getMapY()*OBJECTUNIT - WIN_SIZE.y){
                // Player ist so weit unten, dass er nicht mehr zentriert angezeigt werden kann.
                yfrom = level.getMapY()*OBJECTUNIT - WIN_SIZE.y;
            }
            
            view.reset(sf::FloatRect(xfrom, yfrom, WIN_SIZE.x, WIN_SIZE.y ));

            // Clear screen
            window.clear(sf::Color::White);
            window.setView(view);


            // Render Objects in View
            float xfrom_oj = xfrom / OBJECTUNIT;
            float yfrom_oj = yfrom / OBJECTUNIT;
            uint xfrom_oju = (uint) xfrom_oj;
            uint yfrom_oju = (uint) yfrom_oj;

            //std::cout<< "FROM: ("<< xfrom << ", " <<yfrom << ")";
            //std::cout<< "FLOAT: ("<< xfrom_oj << ", " <<yfrom_oj << ")";
            //std::cout<< "UINT: ("<< xfrom_oju<< ", "<< yfrom_oju<< ")\n";

            //printf("WINSIZE FLOAT: (%f, %f) \t UINT: (%d, %d) \n", wsu.x, wsu.y, xfrom_oju, yfrom_oju);
            for (int x = 0; x < WIN_SIZE.x / OBJECTUNIT + 1; x++){
                for (int y = 0; y < WIN_SIZE.y / OBJECTUNIT + 1; y++){
                    Object* m = level.getObject(sf::Vector2u(x+ xfrom_oju, y+ yfrom_oju));
                    //printf("RENDERING (%d, %d)\n", x, y);
                    if (m == nullptr) continue;
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
