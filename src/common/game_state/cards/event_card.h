//
// Created by daniel on 08.11.2025.
//

#ifndef FRANTIC_EVENT_CARD_H
#define FRANTIC_EVENT_CARD_H

#include <string>
#include "../../serialization/unique_serializable.h"
#include "../../serialization/serializable_value.h"
#include "../../../../rapidjson/include/rapidjson/document.h"
#include "event_card_collection.h"
#include "../player/player.h"

class game_state;

class event_card : public unique_serializable {
private:
    /* determined by string for maintainability
    */
    Event_card_collection event_name;
    //serializable_value<string>* _type;


    // from_diff constructor
    event_card(std::string id);
    // deserialization constructor
    event_card(std::string id, serializable_value<uint>* typ);
public:
    /// Public constructor.
    event_card(Event_card_collection typ);
    /// Public destructor.
    ~event_card();

    /// Accessor for the event of the card.
    Event_card_collection get_event_name() const noexcept;

// card functions
    /// Executes the respective special effect.
    void effect(game_state* game_state, std::string& err);
    /// Serialization implementation.
    void write_into_json(rapidjson::Value& json, rapidjson::Document::AllocatorType& allocator) const override;
    /// Deserialization implementation.
    static event_card* from_json(const rapidjson::Value& json);
};


#endif //FRANTIC_EVENT_CARD_H
