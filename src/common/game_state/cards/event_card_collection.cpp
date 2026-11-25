//
// Created by daniel on 19.11.25.
//

#include "event_card_collection.h"
#include <string>

std::map<Event_card_collection, std::string> EventToString = {
    {Event_card_collection::DOOMSDAY, "DOOMSDAY" },
    {Event_card_collection::EARTHQUAKE, "EARTHQUAKE" },
    {Event_card_collection::FINISH_LINE, "FINISH_LINE" },
    {Event_card_collection::TIME_BOMB, "TIME_BOMB"},
    {Event_card_collection::COMMUNISM, "COMMUNISM"},
    {Event_card_collection::EXPANSION, "EXPANSION"},
    {Event_card_collection::CAPITALISM, "CAPITALISM"},
    {Event_card_collection::THIRD_TIME_LUCKY, "THIRD_TIME_LUCKY"},
    {Event_card_collection::MEXICAN_STANDOFF, "MEXICAN_STANDOFF"},
    {Event_card_collection::ROBIN_HOOD,  "ROBIN_HOOD"  },
    {Event_card_collection::MATING_SEASON,  "MATING_SEASON"  },
    {Event_card_collection::FRIDAY_THE_13TH,  "FRIDAY_THE_13TH"  }
};
std::string getEventCardString(Event_card_collection e) {
     return EventToString[e];
};
