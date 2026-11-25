//
// Created by Manuel on 25.01.2021.
//

#ifndef FRANTIC_DRAW_PILE_H
#define FRANTIC_DRAW_PILE_H

#include "card.h"
#include <vector>
#include <string>
#include <algorithm>
#include "../../serialization/serializable.h"
#include "../../serialization/unique_serializable.h"
#include "../../serialization/serializable_value.h"
#include "../../game_state/player/player.h"
#include "../../../../rapidjson/include/rapidjson/document.h"


class draw_pile : public unique_serializable {
private:
    std::vector<card*> _cards;

    /*
     * Deserialization constructor
     */
    draw_pile(std::string id, std::vector<card*>& cards);

    // from_diff constructor
    draw_pile(std::string id);

// pile functions
    void shuffle();

public:
    /// Default constructor that generates a new deck of cards.
    draw_pile();
    /// Constructor that includes only the passed vector of card-pointers and does not generate new ones.
    draw_pile(std::vector<card*>& cards);
    /// Constructor to construct a new draw pile from the reshuffled cards from the discard pile. Required because passing a pointer to the passed vector is more convenient.
    draw_pile(std::vector<card*>* cards);
    /// Public destructor.
    ~draw_pile();


// accessors
    ///Returns true if the draw pile is empty. Required for reshuffling.
    bool is_empty() const noexcept;
    /// Returns the number of cards.
    int get_nof_cards() const noexcept;

#ifdef FRANTIC_SERVER
// state update functions
    /// Fills the pile with all cards of the game.
    void setup_game(std::string& err);
    /// Passes the top card to the player the argument points to.
    void draw(player* player);
    /// Removes the top card. Required at initialization to transfer the top card to the discard pile.
    card* remove_top(std::string& err);
#endif

    /// Serialization implementation.
    virtual void write_into_json(rapidjson::Value& json, rapidjson::Document::AllocatorType& allocator) const override;
    /// Deserialization implementation.
    static draw_pile* from_json(const rapidjson::Value& json);
};
#endif //FRANTIC_DRAW_PILE_H
