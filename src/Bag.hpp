#ifndef BAG_HPP
#define BAG_HPP
#include "Object.hpp"

class Bag {
public:
    Object* obj;
    Bag* next, previous;

    Bag(Object* o) : obj(o)
    {
        next=nullptr;
        previous=nullptr;
    };
    put(Object* o){
        if(next == nullptr) {
            next = new Bag(o);
        } else {
            next.put(o);
            previous = this;
        }
    };
    open(){
        Bag* b = this;
        int i;
        do{
            std::cout << "Item #" <<i<< ": "<< &b.obj;
            b = b.next;
        }while(b.next != nullptr)
    }
    
}

#endif //BAG_HPP