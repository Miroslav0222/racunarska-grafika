
#include "Taster.h"


Taster::Taster(float bx, float by, float tx, float ty)
        : bottom_x(bx), bottom_y(by), top_x(tx), top_y(ty) {}

    bool Taster::contains(float x, float y) const {
        return (x >= bottom_x && x <= top_x && y >= bottom_y && y <= top_y);
    }

    void Taster::printBounds() const {
        std::cout << "Rectangle bounds: "
            << "Bottom-left (" << bottom_x << ", " << bottom_y << "), "
            << "Top-right (" << top_x << ", " << top_y << ")" << std::endl;
    }