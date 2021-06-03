#ifndef FIRST_GAME_H
#define FIRST_GAME_H

#include <SFML/Graphics.hpp>
#include "Level.h"

// Stellt ein Spielfenster dar. Theoretisch wären mehrere Games (multiplayer auf selbem Rechner) möglich.

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
