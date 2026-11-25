//
// Created by daniel on 08.11.2025.
//

#ifndef FRANTIC_EVENT_DRAW_PILE_H
#define FRANTIC_EVENT_DRAW_PILE_H

#include "event_card.h"
#include <vector>
#include <string>
#include <algorithm>
#include "../../serialization/serializable.h"
#include "../../serialization/unique_serializable.h"
#include "../../serialization/serializable_value.h"
#include "../../game_state/player/player.h"
#include "../../../../rapidjson/include/rapidjson/document.h"

//! This class is a container of the event cards.

//! Whenever a black card is played, one event card is automatically drawn and transferred to the event discard pile.
//! Unlike the ordinary draw pile, this object has no GUI representation.
class event_draw_pile : public unique_serializable {
private:
    std::vector<event_card*> _event_cards;

    /*
     * Deserialization constructor
     */
    event_draw_pile(std::string id, std::vector<event_card*>& event_cards);

    // from_diff constructor
    event_draw_pile(std::string id);

// pile functions
    void shuffle();

public:
// constructors
    event_draw_pile();
    event_draw_pile(std::vector<event_card*>& event_cards);
    ~event_draw_pile();


    /// Returns true if the event draw pile is empty. Should always return false as long as event cards are included that end the round immediately.
    bool is_empty() const noexcept;
    /// Returns the number of cards on the draw pile.
    int get_nof_cards() const noexcept;

#ifdef FRANTIC_SERVER
    /// Creates a new deck of event cards (one card of each type). The new deck is shuffled after creation.
    void setup_game(std::string& err);
    /// Removes the top card to transfer it to the event discard pile.
    event_card* remove_top(std::string& err);
#endif

    /// Serialization implementation.
    virtual void write_into_json(rapidjson::Value& json, rapidjson::Document::AllocatorType& allocator) const override;
    /// Deserialization implementation.
    static event_draw_pile* from_json(const rapidjson::Value& json);
};


#endif //FRANTIC_EVENT_DRAW_PILE_H
