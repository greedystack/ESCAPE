//
// Created by Marius on 08.01.20.
//

#include "Game.h"

// in Pixel:
const int OBJECTUNIT = 20;
// in ObjectUnit:
const int WINDOWSIZEX=100;
const int WINDOWSIZEY=100;
const int BGSIZE=200; // minimum half of screensize!! // must be divideable by 2

const sf::Time UPDATE_TIME = sf::milliseconds(30); // Latency


Game::Game(uint64_t mX, uint64_t mY, int p)
: mapsizex(mX), mapsizey(mY), playercount(p)
{
    textures = (Tex**)(malloc(TEXTURES_PER_CATEGORY * sizeof(uint8_t) * sizeof(Tex *)));
    loadTextures();
    loadFonts();

    map = (MapObject**)(malloc(((mapsizex * mapsizey) + HEADER) * sizeof(MapObject *)));
    printf("Map allocated \n");
    // Defining Map-Headers:
    map[0]=(MapObject*)HEADER;      // 0: Header-Size
    map[1]=(MapObject*)mapsizey;    // 1: Y-Size of Map

    player = (MapObject**)(malloc(playercount * sizeof(MapObject *)));
    for(short i=0; i<playercount; i++) player[i] = new MapObject(map, getTexture('P', 0), 'P', 5, 10);

    buildBorders();
    //drawBackground(10);

    new MapObject(map, getTexture('P', 0), 'W', 7, 7);
    new MapObject(map, getTexture('P', 0), 'Q', 3, 3);

// sf::Text text("Hello SFML", font, 30);
/*/ Load a music to play
        sf::Music music;
        if (!music.openFromFile("nice_music.ogg"))
            return EXIT_FAILURE;
        // Play the music
        music.play();
         */
}

void Game::loadTextures(){
    // backgrounds
    loadTexture('b', 0, new Tex("../include/img/bg1.jpg", OBJECTUNIT, OBJECTUNIT)); // Testbackground

    // Player
    loadTexture('P', 0, new Tex("../include/img/mario.png", 20, 40));
}

void Game::loadFonts() {
    if (!font.loadFromFile("../include/KulimPark-Regular.ttf"))
        printf("ERROR: FONT NOT LOADED!");
        //return EXIT_FAILURE;
}

void Game::buildBorders(){
    for (int x=0; x<mapsizex; x++){
        new MapObject(map, getTexture('P', 0), 'W', x, 0);
        new MapObject(map, getTexture('P', 0), 'W', x, mapsizey-1);
    }
    for (int y=1; y<mapsizey-1; y++){
        new MapObject(map, getTexture('P', 0), 'W', 0, y);
        new MapObject(map, getTexture('P', 0), 'W',mapsizex-1, y);
    }
}

void Game::drawBackground(int bgRepeteAfter){
    for (int x=0; x<bgRepeteAfter; x++){
        for (int y=0; y<bgRepeteAfter; y++){
            new MapObject(map, getTexture('b', 0), ' ', x, y);
        }
    }
}

void Game::start(){
    window = new sf::RenderWindow(sf::VideoMode(WINDOWSIZEX*OBJECTUNIT, WINDOWSIZEY*OBJECTUNIT), "SFML window");
    sf::Clock clock;
    sf::Time elapsed = clock.restart();
    
    // keep track if we have to redraw things. No need if nothing has been updated!
    bool update = true;

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
            }
        }

        // make as many updates as needed for the elapsed time
        while (elapsed > UPDATE_TIME)
        {

            // classical key pressed checks
            if(sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
            {
                player[0]->move(1);
            }
            if(sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
            {
                player[0]->move(2);
            }
            if(sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
            {
                player[0]->move(3);
            }
            if(sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
            {
                player[0]->move(4);
            }

            // don't forget to subtract the updateTime each cycle ;-)
            elapsed -= UPDATE_TIME;
            update = true;
        }

        if(update) render();
    }

    free(map);
    free(textures);
    free(player);
    free(window);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////// RENDERING -> später eigener Fred, execution mit 60 FPS -> wichtig, um wenig Latenz zwischen Aktion und Rendering zu haben und auch Aktion durch System möglich zu machen (nicht nur durch Usereingabe)
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////*/
void Game::render(){

    // Clear screen
    window->clear();
    // Draw the bgSprite
    //window->draw(bgSprite);
    //window.draw(test);

    int xfrom=player[0]->getX()-(WINDOWSIZEX/2);
    int yfrom=player[0]->getY()-(WINDOWSIZEY/2);
    if(xfrom < 0) xfrom = 0;
    if(yfrom < 0) yfrom = 0;
    if(xfrom+WINDOWSIZEX >= mapsizex) xfrom = mapsizex - WINDOWSIZEX;
    if(yfrom+WINDOWSIZEY >= mapsizey) yfrom = mapsizey - WINDOWSIZEY;

    //printf("refreshing window from (%d, %d) to (%d, %d)\n", xfrom, yfrom, xfrom+WINDOWSIZEX, yfrom+WINDOWSIZEY);

    for (int x = 0; x < WINDOWSIZEX; x++){
        for (int y = 0; y < WINDOWSIZEY; y++){
            MapObject* m = map[((x+xfrom)*(uint64_t)map[1])+(y+yfrom)+(uint64_t)map[0]];
            if (m == nullptr) continue;
            //if (typeid(*n) != typeid(VisibleObject)) continue;
            //VisibleObject* m = (VisibleObject*)n;
            //printf("RENDERING (%d, %d, %d) [%d]\n", x, y, z, m);

            if(m->isVisible()){
                m->sprite.setPosition((float)x*OBJECTUNIT, (float)y*OBJECTUNIT);
                window->draw(m->sprite);
            }

            // draw also onMe Stack:
            while (m->getOnMe() != nullptr) {
                m=m->getOnMe();
                if(!m->isVisible()) continue;
                m->sprite.setPosition((float)x*OBJECTUNIT, (float)y*OBJECTUNIT);
                window->draw(m->sprite);
            }
            //usleep(10000);
        }
    } //*/
    // Draw the string
    //window.draw(text);
    // Update the window
    window->display();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////// EVENTS
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Game::catchEvents(){
    sf::Event event;
    while (window->pollEvent(event))
    {
        switch (event.type)
        {
            case sf::Event::Closed:
                window->close();
                break;

            case sf::Event::KeyPressed:
                switch(event.key.code)
                {
                    case sf::Keyboard::Escape:
                        //test.setColor(sf::Color(255, 0, 0, 128));
                        break;
                    case sf::Keyboard::Space:
                        //test.setColor(sf::Color(0, 255, 0, 128));
                        break;
                    case sf::Keyboard::Enter:
                        //test.setColor(sf::Color(0, 0, 255, 128));
                        break;
                    case sf::Keyboard::Left:
                        player[0]->move(1);
                        break;
                    case sf::Keyboard::Up:
                        player[0]->move(2);
                        break;
                    case sf::Keyboard::Right:
                        player[0]->move(3);
                        break;
                    case sf::Keyboard::Down:
                        player[0]->move(4);
                        break;
                    default:
                        break;
                }
                break;
            default:
                break;
        }
    }
}

