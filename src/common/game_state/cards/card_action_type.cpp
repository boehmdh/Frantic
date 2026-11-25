#include "card_action_type.h"
#include <algorithm>

static std::map<CardActionType, std::string> ActionToString = {
    {CardActionType::NONE,    "NONE"},
    {CardActionType::SKIP,    "SKIP"},
    {CardActionType::REVERSE, "REVERSE"},
    {CardActionType::FANTASTIC_FOUR, "FANTASTIC_FOUR"},
    {CardActionType::EQUALITY, "EQUALITY"},
    {CardActionType::SECOND_CHANCE, "SECOND_CHANCE"},
};

std::string action_to_string(CardActionType a) {
    return ActionToString[a];
}

CardActionType string_to_action(const std::string& s) {
    for (auto& p : ActionToString) {
        if (p.second == s)
            return p.first;
    }
    return CardActionType::NONE;
}

