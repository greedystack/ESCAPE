//#include <iostream>

#include <string>
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include "Game.h"
#include <unistd.h>

const int WINDOWSIZEX=100;
const int WINDOWSIZEY=100;

    int main()
    {
        // Create the main window

        // Load a bgSprite to display
        sf::Texture texture;
        if (!texture.loadFromFile("../include/img/bg1.jpg"))
            return EXIT_FAILURE;
        texture.setRepeated(true);
        sf::Sprite bgSprite(texture);




////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////// Create Game
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



        // todo: Popup-Menu-Klasse (wie Itemlist) -> Verkettete Listen, aus denen man menus oder items wählen kann mit isVisible bool
        // todo: Item-Klasse für Items und settings -> individuelle action-Methode (Interface?)

        Game game(64, 64, 1);
        game.start();


        return EXIT_SUCCESS;
    }
