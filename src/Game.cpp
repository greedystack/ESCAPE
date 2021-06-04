
#include "Game.h"
#include <iostream>

// in Pixel:
const int OBJECTUNIT = 20;
// in ObjectUnit:
const int WINDOWSIZEX=100;
const int WINDOWSIZEY=100;
const int BGSIZE=200; // minimum half of screensize!! // must be divideable by 2

const sf::Time UPDATE_TIME = sf::milliseconds(40); // Latency


Game::Game(){
    
}

void Game::start(){
    window = new sf::RenderWindow(sf::VideoMode(WINDOWSIZEX*OBJECTUNIT, WINDOWSIZEY*OBJECTUNIT), "SFML window");
    sf::Clock clock;
    sf::Time elapsed = clock.restart();
    
    // keep track if we have to redraw things. No need if nothing has been updated!
    bool update = true;
    bool paused = false;

    level = new Level(200, 200);

    while (window->isOpen())
    {
        // add the time passed since the last cycle
        elapsed = elapsed + clock.restart();

        //catchEvents();
        sf::Event event;
        while (window->pollEvent(event))
        {
            switch (event.type)
            {
                case sf::Event::Closed:
                    window->close();
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
                    //window->create());
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
                level->stepLeft(level->getPlayer());
                update = true;
            }
            if(sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
            {
                level->stepUp(level->getPlayer());
                update = true;
            }
            if(sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
            {
                level->stepRight(level->getPlayer());
                update = true;
            }
            if(sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
            {
                level->stepDown(level->getPlayer());
                update = true;
            }

            // don't forget to subtract the updateTime each cycle ;-)
            elapsed -= UPDATE_TIME;
        }

        if(update){
            render();
            update = false;
        } 
    }

    free(window);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////// RENDERING -> später eigener Fred, execution mit 60 FPS -> wichtig, um wenig Latenz zwischen Aktion und Rendering zu haben und auch Aktion durch System möglich zu machen (nicht nur durch Usereingabe)
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////*/
void Game::render(){
    // Ist das wirklich effizient so? Gehen Views nicht auch?
    // https://www.sfml-dev.org/tutorials/2.2/graphics-view.php#showing-more-when-the-window-is-resized

    // Clear screen
    window->clear();

    int xfrom=level->getPlayer()->posX-(WINDOWSIZEX/2);
    int yfrom=level->getPlayer()->posY-(WINDOWSIZEY/2);
    if(xfrom < 0) xfrom = 0;
    if(yfrom < 0) yfrom = 0;
    if(xfrom+WINDOWSIZEX >= level->getMapX()) xfrom = level->getMapX() - WINDOWSIZEX;
    if(yfrom+WINDOWSIZEY >= level->getMapY()) yfrom = level->getMapY() - WINDOWSIZEY;

    printf("refreshing window from (%d, %d) to (%d, %d)\n", xfrom, yfrom, xfrom+WINDOWSIZEX, yfrom+WINDOWSIZEY);

    for (int x = 0; x < WINDOWSIZEX; x++){
        for (int y = 0; y < WINDOWSIZEY; y++){
            Object* m = level->getObject(x+xfrom, y+yfrom);
            if (m == nullptr) continue;
            //if (typeid(*n) != typeid(VisibleObject)) continue;
            //VisibleObject* m = (VisibleObject*)n;
            //printf("RENDERING (%d, %d, %d) [%d]\n", x, y, z, m);

            if(m->visible){
                m->sprite.setPosition((float)x*OBJECTUNIT, (float)y*OBJECTUNIT);
                window->draw(m->sprite);
            }
        }
    } //*/


    // Draw the string
    //window.draw(text);


    // Update the window
    window->display();
}


