#ifndef TASTER_H
#define TASTER_H 


#include <iostream>

class Taster {
private:
    float bottom_x; 
    float bottom_y; 
    float top_x;    
    float top_y;    

public:
    Taster(float bx, float by, float tx, float ty);

    bool contains(float x, float y) const;

    void printBounds() const;
};

#endif 