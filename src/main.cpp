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
const sf::Time UPDATE_TIME = sf::milliseconds(15); // Move Speed
sf::Vector2u WIN_SIZE(1024, 1024); // in Pixel
float zoom = 0.5f;

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
                    //std::cout << "-UNKNOWN EVENT: " << event.type << std::endl;
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
            if(sf::Keyboard::isKeyPressed(sf::Keyboard::Return) 
                || sf::Keyboard::isKeyPressed(sf::Keyboard::Enter))
            {
                // Item Bag
                level.getPlayer()->useItem(); // temporarily
                update = true;
            }

            // don't forget to subtract the updateTime each cycle ;-)
            elapsed -= UPDATE_TIME;
        }

        
        if(update){
            //Set View
            
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
