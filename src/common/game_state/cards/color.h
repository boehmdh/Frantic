//
// Created by daniel on 13.11.25.
//

#ifndef FRANTIC_COLOR_H
#define FRANTIC_COLOR_H
#include <map>
#include <string>
#include <sys/types.h>
//! This enum maps he available Colors to uint values.

//! As playing a rainbow card allows to wish either for a color (BLUE, GREEN, RED or YELLOW) or a number (1--9),
//! the underlying uint values are the following to avoid overlap: BLACK: 10, BLUE: 11, GREEN: 12, RED: 13, YELLOW: 14,
//! RAINBOW: 15.
enum class Color : uint {
    BLACK = 10,
    BLUE = 11,
    GREEN = 12,
    RED = 13,
    YELLOW = 14,
    RAINBOW = 15,
    END_COLOR
};
/// Helper function to return a string of the respective Color in capital letters.
std::string getColorString(Color color);

#endif //FRANTIC_COLOR_H