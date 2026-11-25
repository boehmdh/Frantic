//
// Created by daniel on 12.11.2025.
//

#ifndef FRANTIC_EVENT_DISCARD_PILE_H
#define FRANTIC_EVENT_DISCARD_PILE_H

#include <string>
#include <vector>
#include "event_card.h"
#include "../player/player.h"
#include "../../serialization/serializable.h"
#include "../../serialization/serializable_value.h"
#include "../../../../rapidjson/include/rapidjson/document.h"

/// Event discard pile, represented in the GUI. Initially empty.
class event_discard_pile: public unique_serializable {
private:
    std::vector<event_card*> _event_cards;

    event_discard_pile(std::string id);
    event_discard_pile(std::string id, std::vector<event_card*>& event_cards);
public:
    /// Public constructor.
    event_discard_pile();
    /// Public destructor.
    ~event_discard_pile();

// accessors
    /// Returns nullptr is the event discard pile is empty, otherwise a pointer to the top event card. For representation in the GUI.
    const event_card* get_top_card() const;

#ifdef FRANTIC_SERVER
    /// Clears all event cards and creates a new empty pile.
    void setup_game(std::string& err);  // Clears the stack
    /// Plays an event card and calls its effect() function.
    bool play(event_card* drawn_event_card, game_state* gs, std::string& err);
#endif

    /// Serialization implementation.
    virtual void write_into_json(rapidjson::Value& json, rapidjson::Document::AllocatorType& allocator) const override;
    /// Deserialization implementation.
    static event_discard_pile* from_json(const rapidjson::Value& json);
};


#endif //FRANTIC_EVENT_DISCARD_PILE_H
