//
// Created by Manuel on 25.01.2021.
//

#include "player.h"

#include "../../exceptions/FranticException.h"

player::player(std::string name) : unique_serializable() {
    this->_player_name = new serializable_value<std::string>(name);
    this->_has_drawn = new serializable_value<bool>(false);
    this->_score = new serializable_value<int>(0);
    this->_hand = new hand();
}

player::player(std::string id, serializable_value<std::string>* name,
               serializable_value<int>* score, hand *hand, serializable_value<bool>* has_drawn) :
        unique_serializable(id),
        _player_name(name),
        _hand(hand),
        _score(score),
        _has_drawn(has_drawn)
{ }

player::~player() {
    if (_player_name != nullptr) {
        delete _hand;
        delete _player_name;
        delete _score;
        delete _has_drawn;

        _hand = nullptr;
        _player_name = nullptr;
        _score = nullptr;
        _has_drawn = nullptr;
    }
}

#ifdef FRANTIC_SERVER
player::player(std::string id, std::string name) :
        unique_serializable(id)
{
    this->_player_name = new serializable_value<std::string>(name);
    this->_has_drawn = new serializable_value<bool>(false);
    this->_score = new serializable_value<int>(0);
    this->_hand = new hand();
}

std::string player::get_game_id() {
    return _game_id;
}

void player::set_game_id(std::string game_id) {
    _game_id = game_id;
}
#endif


int player::get_score() const noexcept {
    return _score->get_value();
}

std::string player::get_player_name() const noexcept {
    return this->_player_name->get_value();
}

hand* player::get_hand() const noexcept {
    return this->_hand;
}

bool player::has_drawn() const noexcept {
    return this->_has_drawn->get_value();
}

int player::get_nof_cards() const noexcept {
    return _hand->get_nof_cards();
}


#ifdef FRANTIC_SERVER
void player::setup_round(std::string& err) {
    _has_drawn->set_value(false);
    _hand->setup_round(err);
}

void player::wrap_up_round(std::string &err) {
    int cards_value = _hand->get_score();
    int new_score = _score->get_value();
    new_score = _score->get_value() + cards_value;
    _score->set_value(new_score);
}
void player::set_has_drawn(bool has_drawn) {
    _has_drawn->set_value(has_drawn);
};

void player::add_score(int amount) {
    _score->set_value(_score->get_value() + amount);
}

/*
Unlike in Lama this function checks whether the player has already drawn a card and is permitted to fold.
This only ends this players round.
*/
bool player::fold(std::string &err) {

    if (has_drawn()) {
        _has_drawn->set_value(false);
        return true;
    }
    err = "This player has already folded.";
    return false;
}


void player::add_card(card* card) {
    _hand->add_card(card);
}

// This function is only called if playing is permitted.
bool player::remove_card(std::string card_id, card*& card, std::string &err) {
    //card = nullptr;

    return _hand->remove_card(card_id, card, err);
}

#endif


void player::write_into_json(rapidjson::Value& json, rapidjson::Document::AllocatorType& allocator) const {
    unique_serializable::write_into_json(json, allocator);

    rapidjson::Value id_val(_id.c_str(), allocator);
    json.AddMember("id", id_val, allocator);

    rapidjson::Value name_val(rapidjson::kObjectType);
    _player_name->write_into_json(name_val, allocator);
    json.AddMember("player_name", name_val, allocator);

    rapidjson::Value has_folded_val(rapidjson::kObjectType);
    _has_drawn->write_into_json(has_folded_val, allocator);
    json.AddMember("has_drawn", has_folded_val, allocator);

    rapidjson::Value score_val(rapidjson::kObjectType);
    _score->write_into_json(score_val, allocator);
    json.AddMember("score", score_val, allocator);

    rapidjson::Value hand_val(rapidjson::kObjectType);
    _hand->write_into_json(hand_val, allocator);
    json.AddMember("hand", hand_val, allocator);
}


player *player::from_json(const rapidjson::Value &json) {
    if (json.HasMember("id")
        && json.HasMember("player_name")
        && json.HasMember("has_drawn")
        && json.HasMember("hand"))
        
    {
        return new player(
                json["id"].GetString(),
                serializable_value<std::string>::from_json(json["player_name"].GetObject()),
                serializable_value<int>::from_json(json["score"].GetObject()),
                hand::from_json(json["hand"].GetObject()),
                serializable_value<bool>::from_json(json["has_drawn"].GetObject()));
    } else {
        throw FranticException("Failed to deserialize player from json. Required json entries were missing.");
    }
}
