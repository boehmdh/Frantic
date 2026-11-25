//
//
//

#include "discard_pile.h"
#include "../../serialization/vector_utils.h"
#include "../../exceptions/FranticException.h"


discard_pile::discard_pile(std::string id) : unique_serializable(id) { }

discard_pile::discard_pile(std::string id, std::vector<card *> &cards):
        unique_serializable(id),
        _cards(cards)
{ }

discard_pile::discard_pile() : unique_serializable() { }

discard_pile::~discard_pile() {
    for (int i = 0; i < _cards.size(); i++) {
        delete _cards[i];
    }
    _cards.clear();
}


bool discard_pile::can_play(const card *card, std::string& err)  {
    return _cards.empty() || card->can_be_played_on(_cards.back(), err);
}

const card* discard_pile::get_top_card() const  {
    if (_cards.size() > 0) {
        return _cards.back();
    }
    return nullptr;

}


#ifdef FRANTIC_SERVER
void discard_pile::setup_game(std::string &err) {
    // remove all cards (if any) and clear it
    for (int i = 0; i < _cards.size(); i++) {
        delete _cards[i];
    }
    _cards.clear();
}

bool discard_pile::try_play(const std::string& card_id, player* currentPlayer,std::vector<player*>& players, uint wish, std::string& err) {
    card* played_card = nullptr;
    if (currentPlayer->get_hand()->try_get_card(card_id, played_card)) {
        uint playedNumber = played_card->get_value();
        Color playedColor = played_card->get_color();
        
      if (played_card->get_action() == CardActionType::EQUALITY) {
            int current_hand_size = currentPlayer->get_nof_cards();

            bool is_unique_min = true;
            for (size_t i = 0; i < players.size(); ++i) {
                player* p = players[i];

                // eigenen Spieler überspringen
                if (p->get_id() == currentPlayer->get_id()) {
                    continue;
                }

                int p_hand = p->get_nof_cards();

                // wenn ein anderer Spieler gleich viele oder weniger Karten hat, false
                
                if (p_hand <= (current_hand_size-2)) {
                    is_unique_min = false;
                    break;
                }
            }

            if (is_unique_min) {
                err = "Equality card cannot be played because the current player already has the fewest cards.";
                return false;
            }
        }
        // the variable wishCheck is declared for debugging: To discern whether either the which causes try_play()
        // to return FALSE or the card cannot be played on the top card.
        // Gerne die Struktur verbessern, falls ihr Ideen habt! :-)
        bool wishCheck = false;
        if (wish != 0 &&  !(wish == playedNumber || wish == static_cast<uint>(playedColor) || playedColor == Color::RAINBOW)) {
            err = "The desired card with number " + std::to_string(playedNumber) + " and color " + getColorString(playedColor)
              + " does not match the wish.";
            return false;
        }
        if (can_play(played_card, err))  {
            card* local_system_card;
            if (currentPlayer->remove_card(played_card->get_id(), local_system_card, err)) {
                _cards.push_back(local_system_card);

                return true;
            } else {
                err = "Could not play card " + played_card->get_id() + " because player does not have this card.";
            }
        }
    } else {
        err = "The player does not possess the card " + card_id + ", which was requested to be played.";
    }
    return false;
}

void discard_pile::addCard(card* played_card, std::string& err) {
    if (can_play(played_card, err)) {
        _cards.push_back(played_card);
    }
}


std::vector<card*>* discard_pile::get_pile_for_reshuffling() {
    std::vector<card*> new_pile = {_cards.back()};
    _cards.pop_back();
    std::vector<card*>* pile_to_reshuffle = &this->_cards;
    this->_cards = new_pile;
    return pile_to_reshuffle;
}

#endif


discard_pile *discard_pile::from_json(const rapidjson::Value &json) {
    if (json.HasMember("id") && json.HasMember("cards")) {
        std::vector<card*> deserialized_cards = std::vector<card*>();
        for (auto &serialized_card : json["cards"].GetArray()) {
            deserialized_cards.push_back(card::from_json(serialized_card.GetObject()));
        }
        return new discard_pile(json["id"].GetString(), deserialized_cards);
    } else {
        throw FranticException("Could not parse draw_pile from json. 'id' or 'cards' were missing.");
    }
}

void discard_pile::write_into_json(rapidjson::Value &json,
                                   rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator> &allocator) const {
    unique_serializable::write_into_json(json, allocator);
    json.AddMember("cards", vector_utils::serialize_vector(_cards, allocator), allocator);
}





