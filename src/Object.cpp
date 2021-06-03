#include "Object.h"

Object::Object(Tex* t) 
    : tex(t)
{
    sizeX =1;
    sizeY =1;
    if (tex != nullptr){
        visible=true;
        sprite.setTexture(tex->regular);
        sprite.setOrigin(0.5, 0.5);
        //sprite.setRotation((float) rotation*90);
    }
}

void Object::lookInDirection(char dir){
    switch (dir){
        case 'r':
            sprite.setScale(tex->scaleX * (-1), tex->scaleY);
            break;
        case 'l':
            sprite.setScale(tex->scaleX, tex->scaleY);
            break;
        case 'u':
            // Hier fehlen noch Textures
            break;
        default:
        case 'd':
            // Hier fehlen noch Textures
            break;
    }
}