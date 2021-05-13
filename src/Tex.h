//
// Created by Marius on 04.01.20.
//

#ifndef FIRST_TEX_H
#define FIRST_TEX_H


#include <SFML/Graphics/Texture.hpp>
#include <string>
using namespace std;

class Tex {
public:
    sf::Texture regular, moving_start, moving_while, moving_end;
    int x, y;
    float scaleX, scaleY;
    Tex(string, int, int, float=1.f, float=1.f);
    void addMoving(string, string, string);
};

#endif //FIRST_TEX_H
