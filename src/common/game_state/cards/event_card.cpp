//
// Created by daniel on 08.11.2025.
//

#include "event_card.h"
#include "../game_state.h"
#include "../../exceptions/FranticException.h"

// from_diff constructor
event_card::event_card(std::string id) : unique_serializable(id) { }

// deserialization constructor
event_card::event_card(std::string id, serializable_value<uint> *eve)
        : unique_serializable(id), event_name(static_cast<Event_card_collection>(eve->get_value()))
{ }

//public constructor
event_card::event_card(Event_card_collection eve) :
        //unique_serializable(),
        event_name(eve)
{ }

event_card::~event_card() { }


Event_card_collection event_card::get_event_name() const noexcept {
    return event_name;
}


/// Executes the effect of the event card.
///
/// @details
/// This function applies the specific effect of the event card that is played:
///
/// **DOOMSDAY:**
/// - The game immediately ends.
/// - Every player receives 50 points.
///
/// **EARTHQUAKE:**
/// - Every player passes their entire hand of cards to the player on their right.
///
/// **FINISH_LINE:**
/// - The game ends immediately.
/// - All players count the points of the cards remaining in their hands.
///
/// **TIME_BOMB:**
/// - The game lasts for three more turns.
/// - The game ends immediately if a player would take a fourth turn.
/// - If a player empties their hand before the three-turn limit:
///   - That player loses 10 points.
///   - All other players receive a 10-point penalty plus points for the cards in their hand.
/// - If no player finishes the game within the three-turn limit, all points are doubled.
///
/// **EXPANSION:**
/// - The game ends immediately.
/// - All players count the points of the cards remaining in their hands.
///
/// **COMMUNISM:**
/// - All players are adjusted to have the same number of cards as the player with the most cards.
/// - Players with fewer cards draw the necessary number of cards to match the maximum.
///
/// **CAPITALISM:**
///
/// **THIRD_TIME_LUCKY:**
/// - Every player draws three cards from the draw pile.
///
/// **MEXICAN_STANDOFF:**
/// - Every player discards all cards in their hand.
/// - Players then draw three new cards from the draw pile, in turn order.
///
/// **ROBIN_HOOD:**
/// - The player with the fewest cards swaps their entire hand with the player who has the most cards.
///
/// **MATING_SEASON:**
/// - Every player must discard all sets of number cards in their hand.
/// - Sets include pairs, triples, quadruples, etc., regardless of the color of the cards.
///
/// **FRIDAY_THE_13TH:**
/// - It is Friday the 13th, but nothing unusual happens in the game.
/// - The game continues normally without any effect.

void event_card::effect(game_state* game_state, std::string& err) {
#ifdef FRANTIC_SERVER
    switch (event_name)
    {
    //DOOMSDAY event card.
    case Event_card_collection::DOOMSDAY: {
    		for (player* p : game_state->get_players()) {
    			p->add_score(50);
    		}
    		game_state->end_game();
    		break;
	    }
    case Event_card_collection::EARTHQUAKE: {
        auto &players = game_state->get_players();
        const std::size_t n = players.size();
        std::vector<std::vector<card *>> tempHands(n);
        //we take all cards of players
        for (std::size_t i = 0; i < n; ++i) {
            hand *h = const_cast<hand *>(players[i]->get_hand());
            std::vector<std::string> cardIds;
            for (card *c: h->get_cards()) {
                cardIds.push_back(c->get_id());
            }

            //std::string err; //Test weil jetzt string& err übergeben wird
            for (const std::string &id: cardIds) {
                card *removed = nullptr;
                if (h->remove_card(id, removed, err)) {
                    tempHands[i].push_back(removed);
                }

            }
        }
        //give the cards to player on the right
        for (std::size_t i = 0; i < n; ++i) {
            std::size_t right = (i + 1) % n;
            hand *dest = const_cast<hand *>(players[right]->get_hand());
            for (card *c: tempHands[i]) {
                dest->add_card(c);
            }
        }
        break;
    }
    case Event_card_collection::CAPITALISM: {
        auto& players = game_state->get_players();
        const size_t n = players.size();

    // Find index of current player
    player* current = game_state->get_current_player();
    size_t idx = 0;
    for (size_t i = 0; i < n; ++i) {
        if (players[i] == current) {
            idx = i;
            break;
        }
    }

    // Every player draws as many cards as they currently have
    for (size_t offset = 0; offset < n; ++offset) {
        player* p = players[(idx + offset) % n];
        int toDraw = p->get_nof_cards();

        for (int k = 0; k < toDraw; ++k) {
			game_state->draw_card_action(p);
        	/*
        	card* drawn = nullptr;
            if (!game_state->get_draw_pile()->draw(p, drawn, err)) {
                break;
            }
            p->set_has_drawn(true);
        	*/
        }
    }

    break;
    }
    case Event_card_collection::FINISH_LINE: {
            game_state->wrap_up_round(err);
            break;
        }
    case Event_card_collection::TIME_BOMB: {
            game_state->set_time_bomb();
            break;
        }

        case Event_card_collection::EXPANSION: {
            auto& players = game_state->get_players();
            draw_pile* draw = game_state->get_draw_pile();

            const std::size_t n = players.size();
            player* current = game_state->get_current_player();
            std::size_t idx = 0;
            for (std::size_t i = 0; i < n; ++i) {
                if (players[i] == current) {
                    idx = i;
                    break;
                }
            }


            for (std::size_t offset = 0; offset < n; ++offset) {
                player* p = players[(idx + offset) % n];
                int toDraw = static_cast<int>(offset) + 1;
            }
            break;
        }

        case Event_card_collection::MEXICAN_STANDOFF: {
            auto& players = game_state->get_players();
            draw_pile* draw = game_state->get_draw_pile();
            discard_pile* discard = game_state->get_discard_pile();


            for (player* p : players) {
                hand* h = const_cast<hand*>(p->get_hand());
                std::vector<card*> removedCards;
                std::vector<std::string> ids;

                for (card* c : h->get_cards()) {
                    ids.push_back(c->get_id());
                }

                std::string err;
                for (const std::string& id : ids) {
                    card* removed = nullptr;
                    if (h->remove_card(id, removed, err)) {
                        removedCards.push_back(removed);
                    }
                }

                for (card* c : removedCards) {
                    discard->addCard(c, err);
                }

                for (int i = 0; i < 3; i++) {
                    card* drawn = nullptr;
                }
            }
            break;
        }
        case Event_card_collection::THIRD_TIME_LUCKY: {
    		std::vector<player*>& players = game_state->get_players();
    		for (player* p: players) {
    			for (int i = 0; i <=2; ++i)	{
    				game_state->draw_card_action(p);
    			}
    		} break;
    }

        //
        break;
		case Event_card_collection::ROBIN_HOOD: {
		     auto &players = game_state->get_players();
		     const std::size_t n = players.size();
		     if (n < 2) {
		         break;
		     }

		     // Spieler mit min/max Karten finden
		     player* min_player = players[0];
		     player* max_player = players[0];
		     int min_size = players[0]->get_nof_cards();
		     int max_size = min_size;

		     for (std::size_t i = 1; i < n; ++i) {
		         int cnt = players[i]->get_nof_cards();
		         if (cnt < min_size) {
		             min_size = cnt;
		             min_player = players[i];
		         }
		         if (cnt > max_size) {
		             max_size = cnt;
		             max_player = players[i];
		         }
		     }

		     // Wenn alle gleich viele Karten haben oder es derselbe Spieler ist -> kein Effekt muss noch angepasst werden. Entweder zufällig oder auswählen. Keine Ahnung was das beste ist.
		     if (min_player == max_player || min_size == max_size) {
		         break;
		     }

		     hand* min_hand = const_cast<hand*>(min_player->get_hand());
		     hand* max_hand = const_cast<hand*>(max_player->get_hand());

		     // Karten Kopieren
		     std::vector<card*> min_cards = min_hand->get_cards();
		     std::vector<card*> max_cards = max_hand->get_cards();

		     std::string err;

		      // beide Hände löschen
		     for (card* c : min_cards) {
		         card* removed = nullptr;
		         min_hand->remove_card(c->get_id(), removed, err);
		     }
		     for (card* c : max_cards) {
		         card* removed = nullptr;
		         max_hand->remove_card(c->get_id(), removed, err);
		     }

		      // Hände vertauschen
		      for (card* c : min_cards) {
		          max_hand->add_card(c);
		      }
		      for (card* c : max_cards) {
		          min_hand->add_card(c);
		      }

		     break;
		}
		case Event_card_collection::MATING_SEASON:
		{
				for (player* p : game_state->get_players())
				{
				    hand* h = const_cast<hand*>(p->get_hand());
				    std::vector<card*> cards = h->get_cards();


				    int counts[15] = {0};
						//Zählen wieviele Karten von allen Zahlen auf der Hand sind
				    for (card* c : cards)
				    {
				        int v = c->get_value();
				        if (v >= 0 && v < 15)
				            counts[v]++;
				    }


				    std::string dummy_err;
						//Alle Karten mit counts> =2 löschen
				    for (card* c : cards)
				    {
				        int v = c->get_value();
				        if (counts[v] >= 2)
				        {
				            card* removed = nullptr;
				            if (h->remove_card(c->get_id(), removed, dummy_err) && removed != nullptr)
				            {
				                delete removed;   // Karte entfernen --- Karte ist vollständig entfernt. Sie ist nicht auf dem discardpile. Wir benötigen eine force_discard methode in discardpile, wobei die Karten hinter der Schwarzen Karte abgelegt werden.
				            }
				        }
				    }
				}

				break;
		}

		case Event_card_collection::FRIDAY_THE_13TH:
		{
		break;
		}

    // Makes all players have the same number of cards as the player with the most cards.
    case Event_card_collection::COMMUNISM:
		 	{
				auto players = game_state->get_players();

		 		std::vector<int> nof_cards_each_player;
		 		nof_cards_each_player.reserve(players.size());
		 		for (player* p : players) {
		 			nof_cards_each_player.push_back(p->get_nof_cards());
		 		}

		 		int max_nof_cards = *std::max_element(nof_cards_each_player.begin(), nof_cards_each_player.end());

		 		for (player* p : players) {
		 			int nof_cards = p->get_nof_cards();
		 			if (nof_cards < max_nof_cards) {
		 				int diff = max_nof_cards - nof_cards;

		 				for (int i = 0; i < diff; ++i) {
                            game_state->draw_card_action(p);
		 				}
		 			}
		 		}
		 	}
    	break;
    }
#endif
}


// serialization
void event_card::write_into_json(rapidjson::Value &json, rapidjson::Document::AllocatorType& allocator) const {
    unique_serializable::write_into_json(json, allocator);

    rapidjson::Value type_val(rapidjson::kObjectType);
    // The underlying typ of Event_card_collection is uint. The cast is required as serializable_value is not defined
    // for custom defined enums.
    (new serializable_value<uint>(static_cast<uint>(event_name)))->write_into_json(type_val, allocator);
    json.AddMember("event_name", type_val, allocator);
}

// deserialization
event_card *event_card::from_json(const rapidjson::Value &json) {
    if (json.HasMember("id") && json.HasMember("event_name") ) {
        return new event_card(
            json["id"].GetString(),
            serializable_value<uint>::from_json(json["event_name"].GetObject())
            );
    } else {
        throw FranticException("Could not parse json of event_card. Was missing 'id' or 'event_name'.");
    }
}




