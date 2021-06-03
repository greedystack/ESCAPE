#include "Object.h"

Object::Object(Tex* t, int x, int y, int sx, int sy) 
    : tex(t), posX(x), posY(y), sizeX(sx), sizeY(sy) 
{
    if (tex != nullptr){
        visible=true;
        sprite.setTexture(tex->regular);
        sprite.setOrigin(0.5, 0.5);
        //sprite.setRotation((float) rotation*90);
    }
}