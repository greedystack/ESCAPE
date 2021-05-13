//
// Created by Marius on 05.01.20.
//

#include "Tex.h"

Tex::Tex(string link, int sizex, int sizey, float scalex, float scaley): x(sizex), y(sizey), scaleX(scalex), scaleY(scaley) {
    if (!regular.loadFromFile(link,sf::IntRect(0, 0, sizex, sizey))) {
        printf("ERROR! Texture %s not loaded", link.c_str());
    }
}

void Tex::addMoving(string start, string whiile, string end){
    if (!moving_start.loadFromFile(start,sf::IntRect(0, 0, x, y))) {
        printf("ERROR! Texture %s not loaded", start.c_str());
    }
    if (!moving_while.loadFromFile(whiile,sf::IntRect(0, 0, x, y))) {
        printf("ERROR! Texture %s not loaded", whiile.c_str());
    }
    if (!moving_end.loadFromFile(end,sf::IntRect(0, 0, x, y))) {
        printf("ERROR! Texture %s not loaded", end.c_str());
    }
}