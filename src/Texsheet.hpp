
#ifndef TEX_HPP
#define TEX_HPP

#include <SFML/Graphics/Texture.hpp>
#include <string>

class Texsheet {
public:
    sf::Texture texture;

private:
    sf::Vector2u imageCount; // Spritesheets haben mehrere Bilder, wenn sie animiert sind.
    //sf::Texture texture;

public:
    Texsheet(std::string resource, uint x=1, uint y=1)
        : imageCount(x, y)
    {
        if (!texture.loadFromFile(resource)) {
            printf("ERROR! Texture %s not loaded", resource.c_str());
        }
    };

    sf::Vector2u getImageCount(){return imageCount;};
    sf::Vector2u getSize(){return sf::Vector2u(texture.getSize().x / imageCount.y, texture.getSize().y / imageCount.y);};

    //TODO Funktion zum Returnen eines bestimmten Bildes aus Spritesheet.
};

#endif //TEX_HPP
