//
//
//

#include "card.h"
#include "../game_state.h"
#include "../../exceptions/FranticException.h"

// from_diff constructor
card::card(std::string id) : unique_serializable(id) { }

// deserialization constructor
card::card(std::string id, serializable_value<uint> *val, serializable_value<uint> *col)
        : unique_serializable(id), _value(val), _color(static_cast<Color>(col->get_value()))
{ }

//public constructor
card::card(uint val, Color col) :
        unique_serializable(),
        _value(new serializable_value<uint>(val)),
        _color(col),
        _action(CardActionType::NONE)
{}
//Actioncardconstructor
card::card(Color col, CardActionType act)
    : unique_serializable(),
      _value(new serializable_value<uint>(0)),
      _color(col),
      _action(act)
{
}




card::~card() { }


uint card::get_value() const noexcept {
    return _value->get_value();
}

Color card::get_color() const noexcept {
    return _color;
}

CardActionType card::get_action() const noexcept {
    return _action;
}

// overloading for enabling to call the function without err argument (for testing)
bool card::can_be_played_on(const card *const other) const noexcept
{
    // return true if: either number or color matches (except for black (color 0)), else false
    Color this_col = this->get_color();
    Color other_col = other->get_color();
    uint this_val = this->get_value();
    uint other_val = other->get_value();
    // The function returns always true to play on a RAINBOW card to enable playing if this
    // card is the first one on the discard pile. The check for the wish is performed beforehand and this function is
    // called iff the wish matches.
    if (this_col == Color::RAINBOW || other_col == Color::RAINBOW)
    {
        return true;
    }
    if (other_col == Color::BLACK && this_col == Color::BLACK && this_val != other_val) {
        return false;
    }
    if (this_val == other_val || (other_col != Color::BLACK && this_col == other_col)) {
        return true;
    }
    // Note for the last predicate:
    return false;
}

    // actually used function
bool card::can_be_played_on(const card *const other, std::string& err) const noexcept {
    // return true if: either number or color matches (except for black (color 0)), else false
    Color this_col = this->get_color();
    Color other_col = other->get_color();
    uint this_val = this->get_value();
    uint other_val = other->get_value();
    CardActionType this_action_type = this->get_action();
    CardActionType other_action_type = other->get_action();
    // The function returns always true to play on a RAINBOW card to enable playing if this
    // card is the first one on the discard pile. The check for the wish is performed beforehand and this function is
    // called iff the wish matches.
    if (this_col == Color::RAINBOW || other_col == Color::RAINBOW) {
        return true;
    }
    if (other_col == Color::BLACK && this_col == Color::BLACK && this_val != other_val) {
        err = "In Frantic, black is no color and only a card with a matching number may be played upon a black card.";
        return false;
    }
    if (this_action_type == CardActionType::NONE && other_action_type == CardActionType::NONE) { // exclude special cards
        if (this_val == other_val || (other_col != Color::BLACK && this_col == other_col)) {
            return true;
        }
        err = "The played card with color " + getColorString(this_col) + " and number " + std::to_string(this_val) +
             " cannot be played upon color " + getColorString(other_col) + " and number " + std::to_string(other_val) + ".";
        // Note for the last predicate:
        return false;
    }
    if (this_col == other_col || this_action_type == other_action_type) {
        return true;
    }
    err = "This card does not match the required color or type.";
    return false;

}
uint card::get_point_value() const {
    uint value = _value->get_value();
    if ((value >= 1 && value <= 9) || value == 42) {// the fuck you-card has 42 points
        return value;
    }
    return 7;
}







void card::write_into_json(rapidjson::Value &json, rapidjson::Document::AllocatorType& allocator) const {
    unique_serializable::write_into_json(json, allocator);

    rapidjson::Value value_val(rapidjson::kObjectType);
    _value->write_into_json(value_val, allocator);
    json.AddMember("value", value_val, allocator);

    rapidjson::Value color_val(rapidjson::kObjectType);
    (new serializable_value<uint>(static_cast<uint>(_color)))->write_into_json(color_val, allocator);
    json.AddMember("color", color_val, allocator);
    
    rapidjson::Value action_val(rapidjson::kObjectType);
    (new serializable_value<uint>(static_cast<uint>(_action)))->write_into_json(action_val, allocator);
    json.AddMember("action", action_val, allocator);
}
// deserialization

card *card::from_json(const rapidjson::Value &json) {
    if (json.HasMember("id") && json.HasMember("value") && json.HasMember("color")) {

        auto* val = serializable_value<uint>::from_json(json["value"].GetObject());
        auto* col = serializable_value<uint>::from_json(json["color"].GetObject());

        card* c = new card(
            json["id"].GetString(),
            val,
            col
        );


        if (json.HasMember("action")) {
            auto* act = serializable_value<uint>::from_json(json["action"].GetObject());
            c->set_action(static_cast<CardActionType>(act->get_value()));

        }

        return c;
    }
    throw FranticException("Could not parse json of card. Was missing 'id' or 'val'.");

}





