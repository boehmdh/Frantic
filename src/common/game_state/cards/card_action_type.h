//
// Created by daniel on 01.12.25.
//

#ifndef FRANTIC_CARD_ACTION_H
#define FRANTIC_CARD_ACTION_H
#include <string>
#include <map>
#include <locale>

/// Defines the action type of a card. For non-special cards CardActionType::NONE (uint 0).
enum class CardActionType : uint {
    NONE = 0,
    SKIP,
    REVERSE,
    FANTASTIC_FOUR,
    EQUALITY,
    SECOND_CHANCE,
    END
};

/// Helper function to return a string of the action type in capital letters.
std::string action_to_string(CardActionType a);
/// Helper function to convert a string to the action type.
CardActionType string_to_action(const std::string& s);

#endif //FRANTIC_CARD_ACTION_H


