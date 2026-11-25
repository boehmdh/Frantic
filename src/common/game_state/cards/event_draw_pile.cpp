//
// Created by daniel on 08.11.2025.
//

#include "event_draw_pile.h"


#include "../../serialization/vector_utils.h"
#include "../../exceptions/FranticException.h"


// deserialization constructor
event_draw_pile::event_draw_pile(std::string id, std::vector<event_card*> &event_cards)
        : unique_serializable(id),
          _event_cards(event_cards)
{ }

// from_diff constructor
event_draw_pile::event_draw_pile(std::string id) : unique_serializable(id) { }


event_draw_pile::event_draw_pile(std::vector<event_card*> &event_cards)
        : unique_serializable(), _event_cards(event_cards)
{ }


event_draw_pile::event_draw_pile() : unique_serializable() { }

event_draw_pile::~event_draw_pile() {
    for (event_card* & _event_card : _event_cards) {
        delete _event_card;
    }
    _event_cards.clear();
}


void event_draw_pile::shuffle() {
    std::shuffle(_event_cards.begin(), _event_cards.end(), std::mt19937(std::random_device()()));
}

bool event_draw_pile::is_empty() const noexcept  {
    return _event_cards.empty();
}

int event_draw_pile::get_nof_cards() const noexcept  {
    return _event_cards.size();
}


#ifdef FRANTIC_SERVER
void event_draw_pile::setup_game(std::string &err) {
    // remove all event_cards (if any) and add the change to the "event_cards" array_diff
    for (int i = 0; i < _event_cards.size(); i++) {
        delete _event_cards[i];
    }
    _event_cards.clear();

    // add a fresh set of cards
    for (Event_card_collection ec = static_cast<Event_card_collection>(0); ec != Event_card_collection::END;
        ec = static_cast<Event_card_collection>(static_cast<uint>(ec) + 1)) { //numbers
             _event_cards.push_back(new event_card(ec));
            }
    // shuffle them
    event_draw_pile::shuffle();

}
/*
bool event_draw_pile::uncover(player* player, event_card* uncovered_event_card, std::string& err)  {
    // The game would halt if the event draw pile is empty and no card can be drawn. This case could occur depending on
    // the event cards that are actually implemented. To this. the draw pile can
    // simply re-initiated, simulating reshuffling of the discard pile.
    if (_event_cards.empty()) {
           this->setup_game(err);
        }
    uncovered_event_card = _event_cards.back();
    if (uncovered_event_card != nullptr) {
            _event_cards.pop_back();
            printf("event card removed from draw pile");
            return true;
        }
    else {
        err = "Could not uncover event card in event_draw_pile::uncover(layer* player, event_card* uncovered_event_card, std::string& err).";
    }
    return false;
}
*/

event_card* event_draw_pile::remove_top(std::string& err) {
    event_card* drawn_event_card = nullptr;
    if (_event_cards.empty()) {
        this->setup_game(err);
    }
    if (!_event_cards.empty()) {
        drawn_event_card = _event_cards.back();
        _event_cards.pop_back();
    } else {
        err = "Could not draw event card because event_drawpile is empty.";
    }
    return drawn_event_card;
}

#endif


void event_draw_pile::write_into_json(rapidjson::Value &json, rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator> &allocator) const {
    unique_serializable::write_into_json(json, allocator);
    json.AddMember("event_cards", vector_utils::serialize_vector(_event_cards, allocator), allocator);
}


event_draw_pile *event_draw_pile::from_json(const rapidjson::Value &json) {
    if (json.HasMember("id") && json.HasMember("event_cards")) {
        std::vector<event_card*> deserialized_event_cards = std::vector<event_card*>();
        for (auto &serialized_event_card : json["event_cards"].GetArray()) {
            deserialized_event_cards.push_back(event_card::from_json(serialized_event_card.GetObject()));
        }
        return new event_draw_pile(json["id"].GetString(), deserialized_event_cards);
    } else {
        throw FranticException("Could not parse event_draw_pile from json. 'id' or 'event_cards' were missing.");
    }
}
