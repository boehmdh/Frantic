//
// Created by daniel on 12.11.2025.
//

#include "event_discard_pile.h"
#include "../../serialization/vector_utils.h"
#include "../../exceptions/FranticException.h"


event_discard_pile::event_discard_pile(std::string id) : unique_serializable(id) { }

event_discard_pile::event_discard_pile(std::string id, std::vector<event_card *> &event_cards):
        unique_serializable(id),
        _event_cards(event_cards)
{ }

event_discard_pile::event_discard_pile() : unique_serializable() { }

event_discard_pile::~event_discard_pile() {
    for (int i = 0; i < _event_cards.size(); i++) {
        delete _event_cards[i];
    }
    _event_cards.clear();
}


const event_card* event_discard_pile::get_top_card() const  {
    if (_event_cards.size() > 0) {
        return _event_cards.back();
    }
    return nullptr;

}

#ifdef FRANTIC_SERVER
void event_discard_pile::setup_game(std::string &err) {
    // remove all cards (if any) and clear it
    for (int i = 0; i < _event_cards.size(); i++) {
        delete _event_cards[i];
    }
    _event_cards.clear();
}

bool event_discard_pile::play(event_card* drawn_event_card, game_state* gs, std::string& err) {
        drawn_event_card->effect(gs, err);
        _event_cards.push_back(drawn_event_card);
        return true;
}
#endif

void event_discard_pile::write_into_json(rapidjson::Value &json,
                                   rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator> &allocator) const {
    unique_serializable::write_into_json(json, allocator);
    json.AddMember("event_cards", vector_utils::serialize_vector(_event_cards, allocator), allocator);
}


event_discard_pile *event_discard_pile::from_json(const rapidjson::Value &json) {
    if (json.HasMember("id") && json.HasMember("event_cards")) {
        std::vector<event_card*> deserialized_event_cards = std::vector<event_card*>();
        for (auto &serialized_event_card : json["event_cards"].GetArray()) {
            deserialized_event_cards.push_back(event_card::from_json(serialized_event_card.GetObject()));
        }
        return new event_discard_pile(json["id"].GetString(), deserialized_event_cards);
    } else {
        throw FranticException("Could not parse event_discard_pile from json. 'id' or 'event_cards' were missing.");
    }
}






