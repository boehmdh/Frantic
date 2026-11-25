//
// Created by daniel on 19.11.25.
//
#include "color.h"

std::map<Color, std::string> ColorToString = {
    {Color::BLUE, "BLUE" },
    {Color::GREEN, "GREEN" },
    {Color::RED, "RED"},
    {Color::YELLOW, "YELLOW"},
    {Color::RAINBOW, "RAINBOW"},
    {Color::BLACK, "BLACK" },
    {Color::END_COLOR, "(no color)"}
};

std::string getColorString(Color color) {
        return ColorToString[color];
    }