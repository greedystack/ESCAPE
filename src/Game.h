#ifndef FIRST_GAME_H
#define FIRST_GAME_H

#include <SFML/Graphics.hpp>
#include "Level.h"

class Game {
public:
    Game();
    void start();

private:
    void render();
    void catchEvents();
    void catchKeypress();
    sf::RenderWindow *window;
    Level *level;
};


#endif //FIRST_GAME_H
