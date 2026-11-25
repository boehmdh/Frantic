//
// Created by daniel on 12.11.25.
//

#ifndef FRANTIC_ENUM_EVENT_CARDS_COLLECTION_H
#define FRANTIC_ENUM_EVENT_CARDS_COLLECTION_H


#include <map>
#include <string>
#include <sys/types.h>

/// This enum hols the implemented event types.
enum class Event_card_collection : uint {
    DOOMSDAY = 0,
    EARTHQUAKE,
    FINISH_LINE,
    TIME_BOMB,
    CAPITALISM,
    EXPANSION,
    COMMUNISM,
    THIRD_TIME_LUCKY,
    MEXICAN_STANDOFF,
    ROBIN_HOOD,
    MATING_SEASON,
    FRIDAY_THE_13TH,
    END //mark the end for loops
};
/// Helper function to return a string of an event type in capital letters.
std::string getEventCardString(Event_card_collection e);

#endif //FRANTIC_ENUM_EVENT_CARDS_COLLECTION_H
