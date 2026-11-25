//
// Created by Manuel on 25.01.2021.
//

#ifndef FRANTIC_CARD_H
#define FRANTIC_CARD_H

#include <string>
#include "../../serialization/unique_serializable.h"
#include "../../serialization/serializable_value.h"
#include "../../../../rapidjson/include/rapidjson/document.h"
#include "color.h"
#include "card_action_type.h"
class game_state;
class player;

class card : public unique_serializable {
    /* int equals the number value,
     * special cards are mapped to integers that do not occur as number values:
    */
    serializable_value<uint>* _value;
    //uint: 10 black, 11 blue, 12 green, 13 red, 14 yellow
    Color _color;
    CardActionType _action = CardActionType::NONE;
    // from_diff constructor
    card(std::string id);
    // deserialization constructor
    card(std::string id, serializable_value<uint>* val, serializable_value<uint> *col);
public:
    /// Constructor for non-special cards.
    card(uint val, Color col);
    /// Constructor for special cards.
    card(Color col, CardActionType act);
    ///Public destructor.
    ~card();
    

// accessors
    /// Returns the number value of the card. Equals the number value of number cards, 20 for Rainbow cards and 0 for special cards.
    uint get_value() const noexcept;
    /// Returns the Color of the card defined in the enum Color.
    Color get_color() const noexcept;
    /// Returns the CardActionType of a special card or CardActionType::NONE for all other cards.
    CardActionType get_action() const noexcept;

    /// Returns true if this card can be played upon the card passed as the first argument.
    bool can_be_played_on(const card* other, std::string& err) const noexcept;
    /// Overloaded version of can_be_played_on() for testing without passing the err String as an argument.
    bool can_be_played_on(const card* other) const noexcept;
    //! Determines the point value of a card to calculate the player points at the end of a round.

    //! Returns the number value for number cards, 7 for special cards and 42 for the 'FUCK YOU' card.
    uint get_point_value() const;

    /// Sets the action (required for special cards).
    void set_action(CardActionType action) { 
        this->_action = action; 
    }
    /// Serialization implementation.
    void write_into_json(rapidjson::Value& json, rapidjson::Document::AllocatorType& allocator) const override;
    /// Deserialization implementation.
    static card* from_json(const rapidjson::Value& json);
};


#endif //FRANTIC_CARD_H
