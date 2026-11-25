//
//
//

#ifndef FRANTIC_DISCARD_PILE_H
#define FRANTIC_DISCARD_PILE_H

#include <string>
#include <vector>
#include "card.h"
#include "../player/player.h"
#include "../../serialization/serializable.h"
#include "../../serialization/serializable_value.h"
#include "../../../../rapidjson/include/rapidjson/document.h"
/// This class holds the deck of cards that have been played. The most recently played card is displayed in the GUI.
class discard_pile: public unique_serializable {
private:
    std::vector<card*> _cards;

    discard_pile(std::string id);
    discard_pile(std::string id, std::vector<card*>& cards);
public:
    /// Public constructor.
    discard_pile();
    /// Public destructor.
    ~discard_pile();

// accessors
    bool can_play(const card* card, std::string& err);
    const card* get_top_card() const;


#ifdef FRANTIC_SERVER
// state update functions
    /// Sets up a new discard pile.
    void setup_game(std::string& err);  // Clears the stack
    /// Returns true if the passed card of the passed player could be played on the discard pile.
    bool try_play(const std::string& card_id, player* currentPlayer,std::vector<player*>& players, uint wish, std::string& err);
    /// Add a card. Required to place the first card on the pile at the start of each round.
    void addCard(card* played_card, std::string& err);
    /// Get all cards except the top one to reshuffle and pass them to the draw pile.
    std::vector<card*>* get_pile_for_reshuffling();
#endif

// serializable interface
    /// Serialization implementation.
    virtual void write_into_json(rapidjson::Value& json, rapidjson::Document::AllocatorType& allocator) const override;
    /// Deserialization implementation.
    static discard_pile* from_json(const rapidjson::Value& json);
};


#endif //FRANTIC_DISCARD_PILE_H
