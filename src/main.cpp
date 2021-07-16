//#include <iostream>

#include <string>
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include "Level.hpp"
#include <unistd.h>
#include <iostream>
#include <cstdlib>


const std::string TITLE = "Escape!";

const uint OBJECTUNIT = 20; // Pixel pro Map-Block
const sf::Time UPDATE_TIME = sf::milliseconds(90);
const sf::Vector2u MAX_VIEW_SIZE(25,25); // in Mapblocks - um keinen vorteil durch rauszommen oder resizen zu bekommen
const uint MAX_HARDNESS = 5;

sf::Font STANDARDFONT;
void loadFonts() {
    if (!STANDARDFONT.loadFromFile("../include/KulimPark-Regular.ttf"))
        printf("ERROR: FONT NOT LOADED!");
}

void displayText(sf::RenderWindow& window, std::string displaytext){
    sf::View view;
    sf::Text text;
    sf::Sprite img;
    sf::Vector2u WIN_SIZE = window.getSize();

    text.setFont(STANDARDFONT);
    text.setFillColor(sf::Color::White);
    text.setStyle(sf::Text::Bold);

    window.clear(sf::Color::Black);
    view.reset(sf::FloatRect(0, 0, WIN_SIZE.x, WIN_SIZE.y ));
    window.setView(view);

    text.setString(displaytext);
    text.setPosition(WIN_SIZE.x/2, WIN_SIZE.y/2);
    text.setCharacterSize(WIN_SIZE.y/15);
    sf::FloatRect textRect = text.getLocalBounds();
    text.setOrigin(textRect.left + textRect.width/2.0f,
        textRect.top  + textRect.height/2.0f);

    window.draw(text);
    

    window.display();
}

int main(int argc,char* argv[])
{
    sf::Vector2u WIN_SIZE(2000, 2000); // in Pixel
    float zoom = 0.15; // inverted (also im Sinne von Kehrwert)

    int levels_to_play = 3;
    uint hardness = 2;

    if(argc>=2){
        levels_to_play = atoi(argv[0]);
    }
    if(argc>=3){
        hardness = (uint)atoi(argv[1]);
    }

    if(hardness >MAX_HARDNESS) hardness = MAX_HARDNESS;
    if(levels_to_play < 1) levels_to_play = 1;
    else if (levels_to_play > 50) levels_to_play = 50;
    

    loadFonts();

    sf::RenderWindow window(sf::VideoMode(WIN_SIZE.x, WIN_SIZE.y), TITLE);
    sf::View view, statusview;
    

    sf::Clock clock;
    sf::Time elapsed = clock.restart();

    sf::Vector2u start(0,0), end(0,0); // Mapblocks zum Rendern
    
    // keep track if we have to redraw things. No need if nothing has been updated!
    bool updateView = true;
    bool paused = false;
    bool drawWindow = false;
    bool chooseItemMode = false;

    bool won = false;
    bool startmenu = false;
    bool loading = false;

    int lvl_count = levels_to_play;


    Level* level = new Level(10, 10, hardness);
    

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
        

        if(paused || won){
            usleep(250000); // Heizung aus! ;)
            continue;
        } 
        
        // make as many updates as needed for the elapsed time
        while (elapsed > UPDATE_TIME)
        {   
            if(!level->getPlayer()->isParalyzed() && !won){
                ////////////////////////////////////////////////
                //////////////// Level Won
                ////////////////////////////////////////////////
                if(level->getPlayer()->hasWon()){
                    lvl_count--;
                    if(lvl_count <= 0){
                        // WON GAME
                        std::cout << "HURRA! HURRA!" << std::endl;
                        displayText(window, "Congratulations!\nYOU WON!");
                        won=true;
                    }else{
                        std::cout << "init new level" << std::endl;
                        sf::Vector2u lvlsize = level->getDFSSize();
                        delete level;
                        displayText(window, "Loading new Level...");
                        srand (time(NULL));
                        level = new Level(lvlsize.x+ 2*(1+rand()%5), lvlsize.y+ 2*(1+rand()%5), hardness);
                    }
                }else if(level->getPlayer()->wasKilled()){
                    //////////////// Killed. Restart level
                    level->reset();
                }else {

                    ////////////////////////////////////////////////
                    ///// User Input
                    ////////////////////////////////////////////////

                    
                    if(!chooseItemMode){
                        // Actions in Game
                        uint speed = 1;
                        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Period))speed++;
                        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Comma))speed++;
                        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
                        {
                            level->getPlayer()->step(LEFT, speed);
                            updateView = true;
                        }
                        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
                        {
                            level->getPlayer()->step(RIGHT, speed);
                            updateView = true;
                        }
                        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
                        {
                            level->getPlayer()->step(UP, speed);
                            updateView = true;
                        }
                        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
                        {
                            level->getPlayer()->step(DOWN, speed);
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
                            zoom-=0.025;
                            updateView = true;
                        }
                        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Subtract) ||
                            sf::Keyboard::isKeyPressed(sf::Keyboard::N))
                        {
                            zoom+=0.025;
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


                    }else{
                        // Actions in ItemBag
                        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
                        {
                            level->getPlayer()->itempanel.up();
                            elapsed=sf::milliseconds(0);
                            drawWindow=true;
                        }
                        
                        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
                        {
                            level->getPlayer()->itempanel.down();
                            elapsed=sf::milliseconds(0);
                            drawWindow=true;
                        }
                        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Return) 
                        || sf::Keyboard::isKeyPressed(sf::Keyboard::Enter))
                        {
                            if(level->getPlayer()->itempanel.use()){
                                chooseItemMode = false;
                                drawWindow=true;
                            }
                            elapsed=sf::milliseconds(0);
                        }
                    }

                    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
                    {
                        chooseItemMode = !chooseItemMode;

                        if(chooseItemMode){
                            if(!level->getPlayer()->itempanel.enableSelection()){
                                chooseItemMode = false;
                            }
                        }
                        else level->getPlayer()->itempanel.disableSelection();
                        elapsed=sf::milliseconds(0);
                        drawWindow=true;
                    }
                    
                }
            }


            elapsed -= UPDATE_TIME;
        }


        ////////////////////////////////////////////////
        ///// Animate
        ////////////////////////////////////////////////
        if(!chooseItemMode && !won){
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
            if(level->getPlayer()->animateNavi(UPDATE_TIME - elapsed)) drawWindow=true;;
        }

        


        

        
        

        ////////////////////////////////////////////////
        //////////////// Update Window
        ////////////////////////////////////////////////
        if((updateView || drawWindow) && !won){
            if(updateView){
                //std::cout<< "Beschleunigung: " << updateTime.asSeconds() << "\n";

                ////////////////////////////////////////////////
                //////////////// Calculate View Size and View
                ////////////////////////////////////////////////

                sf::Vector2f playerpos = level->getPlayer()->sprite.getPosition();
                float xfrom, yfrom;

                // x
                if(level->getMapX() * OBJECTUNIT < WIN_SIZE.x *zoom){
                    zoom = ((float)(level->getMapX() * OBJECTUNIT)) / (float) WIN_SIZE.x;
                    xfrom = 0;
                }
                else if(playerpos.x < WIN_SIZE.x *zoom *0.5){
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
                if(level->getMapY() * OBJECTUNIT < WIN_SIZE.y *zoom){
                    zoom = ((float)(level->getMapY() * OBJECTUNIT)) / (float) WIN_SIZE.y;
                    yfrom = 0;
                }
                else if(playerpos.y < WIN_SIZE.y *zoom *0.5){
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


                // MAX_VIEW_SIZE (damit Player nicht zu viel sieht):
                // TODO : Debuggen. Und vielleicht dirkt in zoom-befehl legen, damit es nicht ruckelt
                /*
                if((float)(WIN_SIZE.x*zoom - xfrom) / (float) OBJECTUNIT 
                    > MAX_VIEW_SIZE.x)
                {
                    zoom = (float)(MAX_VIEW_SIZE.x*OBJECTUNIT + xfrom) / (float) WIN_SIZE.x;
                }

                if((float)(WIN_SIZE.y*zoom - yfrom) / (float) OBJECTUNIT 
                    > MAX_VIEW_SIZE.y)
                {
                    zoom = (float)(MAX_VIEW_SIZE.y*OBJECTUNIT + yfrom) / (float) WIN_SIZE.y;
                }
                */


                view.reset(sf::FloatRect(xfrom, yfrom, WIN_SIZE.x*zoom, WIN_SIZE.y*zoom ));

                // Adjust Render from-to
                start = sf::Vector2u(
                    xfrom/OBJECTUNIT,
                    yfrom/OBJECTUNIT
                );
                end= sf::Vector2u(
                    ((xfrom + WIN_SIZE.x*zoom)/OBJECTUNIT),
                    ((yfrom + WIN_SIZE.y*zoom)/OBJECTUNIT)
                );

                // Überhangobjekt
                if(end.x < level->getMapX() ) end.x++;
                if(end.y < level->getMapY() ) end.y++;
            }


            ////////////////////////////////////////////////
            //////////////// Draw Window
            ////////////////////////////////////////////////
            // Clear screen
            window.clear(sf::Color::Black);
            window.setView(view);


            
            window.draw(level->getBackground());


            // Render only Objects inside View
            for (uint x = start.x; x < end.x; x++){
                for (uint y = start.y; y < end.y; y++){
                    
                    if(level->getPlayer()->isMarked(x, y)){
                        Object marker = level->getMarkerTemplate(x, y);
                        window.draw(marker.sprite);
                    }
                    
                    
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

            Object* nav = level->getPlayer()->getNavi(
                level->getDirection(level->getPlayer()->pos.x, level->getPlayer()->pos.y)
            );
            if( nav != nullptr) window.draw(nav->sprite);

            window.draw(level->getPlayer()->sprite);

            ////////////////////////////////////////////////
            //// Status View
            ////////////////////////////////////////////////
            
            statusview.reset(sf::FloatRect(0, 0, (float) WIN_SIZE.x, (float) WIN_SIZE.y));
            window.setView(statusview);

            level->getPlayer()->itempanel.draw(window);

            ////////////////////////////////////////////////
            // Update the window
            window.display();
            updateView = false;
        }
        ////////////////////////////////////////////////
    }

    delete level;

    return EXIT_SUCCESS;
}
