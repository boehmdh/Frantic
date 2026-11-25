//
//

#include "game_state.h"

#include "../exceptions/FranticException.h"
#include "../serialization/vector_utils.h"

// from_diff constructor
game_state::game_state(std::string id) : unique_serializable(id) {
    this->_draw_pile = new draw_pile();
    this->_discard_pile = new discard_pile();
    this->_event_draw_pile = new event_draw_pile();
    this->_event_discard_pile = new event_discard_pile();
    this->_players = std::vector<player*>();
    this->_is_started = new serializable_value<bool>(false);
    this->_is_finished = new serializable_value<bool>(false);
    this->_current_player_idx = new serializable_value<int>(0);
    this->_current_turn_idx = new serializable_value<int>(0);
    this->_play_direction = new serializable_value<int>(1);
    this->_round_number = new serializable_value<int>(0);
    this->_starting_player_idx = new serializable_value<int>(0);
    this->_wish = new serializable_value<uint>(0);
    this->_final_turn = new serializable_value<int>(-1);
}
// deserialization constructor
game_state::game_state(std::string id, draw_pile *draw_pile, discard_pile *discard_pile,
                       event_draw_pile *event_draw_pile, event_discard_pile *event_discard_pile,
                       std::vector<player *> &players, serializable_value<bool> *is_started,
                       serializable_value<bool> *is_finished, serializable_value<int> *current_player_idx,
                       serializable_value<int> *current_turn_idx, serializable_value<int> *play_direction,
                       serializable_value<int>* round_number, serializable_value<int> *starting_player_idx,
                       serializable_value<uint> *wish, serializable_value<int> *final_turn)
        : unique_serializable(id),
          _draw_pile(draw_pile),
          _discard_pile(discard_pile),
          _event_draw_pile(event_draw_pile),
          _event_discard_pile(event_discard_pile),
          _players(players),
          _is_started(is_started),
          _is_finished(is_finished),
          _current_player_idx(current_player_idx),
          _current_turn_idx(current_turn_idx),
          _play_direction(play_direction),
          _round_number(round_number),
          _starting_player_idx(starting_player_idx),
          _wish(wish),
          _final_turn(final_turn)
{ }


//public constructor
game_state::game_state() : unique_serializable() {
    this->_draw_pile = new draw_pile();
    this->_discard_pile = new discard_pile();
    this->_event_draw_pile = new event_draw_pile();
    this->_event_discard_pile = new event_discard_pile();
    this->_players = std::vector<player*>();
    this->_is_started = new serializable_value<bool>(false);
    this->_is_finished = new serializable_value<bool>(false);
    this->_current_player_idx = new serializable_value<int>(0);
    this->_current_turn_idx = new serializable_value<int>(0);
    //the Swiss default play direction (1) is anticlockwise, (-1 is clockwise)
    this->_play_direction = new serializable_value<int>(1);
    this->_round_number = new serializable_value<int>(0);
    this->_starting_player_idx = new serializable_value<int>(0);
    // the wish is encoded by an int, 0 for no wish active, otherwise a positive integer for a color or number
    this->_wish = new serializable_value<uint>(0);
    this->_final_turn = new serializable_value<int>(-1);
}



game_state::~game_state() {
    if (_is_started != nullptr) {
        delete _is_started;
        delete _is_finished;
        delete _draw_pile;
        delete _discard_pile;
        delete _event_draw_pile;
        delete _event_discard_pile;
        delete _current_player_idx;
        delete _current_turn_idx;
        delete _starting_player_idx;
        delete _play_direction;
        delete _round_number;
        delete _wish;
        delete _final_turn;

        _is_started = nullptr;
        _is_finished = nullptr;
        _draw_pile = nullptr;
        _discard_pile = nullptr;
        _event_draw_pile = nullptr;
        _event_discard_pile = nullptr;
        _current_player_idx = nullptr;
        _current_turn_idx = nullptr;
        _starting_player_idx = nullptr;
        _play_direction = nullptr;
        _round_number = nullptr;
        _wish = nullptr;
        _final_turn = nullptr;
    }
}

// accessors

bool game_state::is_full() const {
    return _players.size() == _max_nof_players;
}

bool game_state::is_started() const {
    return _is_started->get_value();
}

bool game_state::is_finished() const {
    return _is_finished->get_value();
}


int game_state::get_player_index(player *player) const {
    auto it = std::find(_players.begin(), _players.end(), player);
    if (it == _players.end()) {
        return -1;
    } else {
        return it - _players.begin();
    }
}

bool game_state::is_player_in_game(player *player) const {
    return std::find(_players.begin(), _players.end(), player) < _players.end();
}

bool game_state::is_allowed_to_play_now(player *player) const {
    return player == get_current_player();
}

std::vector<player*>& game_state::get_players() {
    return _players;
}

int game_state::get_current_turn_idx() const {
    return _current_turn_idx->get_value();
}

int game_state::get_play_direction() const {
    return _play_direction->get_value();
}

int game_state::get_round_number() const {
    return _round_number->get_value();
}

uint game_state::get_current_player_idx() const {
    return _current_player_idx->get_value();
}

draw_pile *game_state::get_draw_pile() const {
    return _draw_pile;
}

discard_pile *game_state::get_discard_pile() const {
    return _discard_pile;
}

event_draw_pile *game_state::get_event_draw_pile() const {
    return _event_draw_pile;
}

event_discard_pile *game_state::get_event_discard_pile() const {
    return _event_discard_pile;
}

player* game_state::get_current_player() const {
    if(_current_player_idx == nullptr || _players.size() == 0) {
        return nullptr;
    }
    return _players[_current_player_idx->get_value()];
}

uint game_state::get_wish() const {
    return _wish->get_value();
}

int game_state::get_final_turn() const {
    return _final_turn->get_value();
}

// setters for testing

void game_state::set_draw_pile(draw_pile* draw) {
    _draw_pile = draw;
}

void game_state::set_discard_pile(discard_pile* discard) {
    _discard_pile = discard;
}

void game_state::set_current_player_idx(int idx) {
    if (_current_player_idx->get_value() != idx) {
        _current_player_idx->set_value(idx);
    }
}

void game_state::skip_next_player()
{
    _skip_next_player = true;
}

void game_state::reverse_direction()
{
    int dir = _play_direction->get_value();
    if (dir == 0) dir = 1;
    _play_direction->set_value(-dir);
}
#ifdef FRANTIC_SERVER

// state modification functions without diff
void game_state::setup_round(std::string &err) {

    // update round number
    _round_number->set_value(_round_number->get_value() + 1);

    // setup draw_pile
    _draw_pile->setup_game(err);

    // setup discard_pile
    _discard_pile->setup_game(err);

    // setup event_draw_pile
    _event_draw_pile->setup_game(err);

    // setup event_discard_pile
    _event_discard_pile->setup_game(err);

    // setup players
    for (int i = 0; i < _players.size(); i++) {
        _players[i]->setup_round(err);
        // draw 6 cards
        for (int j = 0; j < 6; j++) {
            _draw_pile->draw(_players[i]);
            std::cerr << err << std::endl;
        }
    }

    // set a first card onto the discard pile
    card* top_card = _draw_pile->remove_top(err);
    if (top_card != nullptr) {
        _discard_pile->addCard(top_card, err);
    }
}

bool game_state::start_game(std::string &err) {

    if (_players.size() < _min_nof_players) {
        err = "You need at least " + std::to_string(_min_nof_players) + " players to start the game.";
        std::cout << "[game_state::start_game] FAIL: " << err << std::endl;
        return false;
    }

    if (!_is_started->get_value()) {
        std::cout << "[game_state::start_game] calling setup_round()" << std::endl;
        this->setup_round(err);
        this->_is_started->set_value(true);
        return true;
    } else {
        err = "Could not start game, as the game was already started";
        std::cout << "[game_state::start_game] FAIL: " << err << std::endl;
        return false;
    }
}


bool game_state::remove_player(player *player_ptr, std::string &err) {
    int idx = get_player_index(player_ptr);
    if (idx != -1) {
        if (idx < _current_player_idx->get_value()) {
            // reduce current_player_idx if the player who left had a lower index
            _current_player_idx->set_value(_current_player_idx->get_value() - 1);
        }
        _players.erase(_players.begin() + idx);
        return true;
    } else {
        err = "Could not leave game, as the requested player was not found in that game.";
        return false;
    }
}


bool game_state::add_player(player* player_ptr, std::string& err) {
    if (_is_started->get_value()) {
        err = "Could not join game, because the requested game is already started.";
        return false;
    }
    if (_is_finished->get_value()) {
        err = "Could not join game, because the requested game is already finished.";
        return false;
    }
    if (_players.size() >= _max_nof_players) {
        err = "Could not join game, because the max number of players is already reached.";
        return false;
    }
    if (std::find(_players.begin(), _players.end(), player_ptr) != _players.end()) {
        err = "Could not join game, because this player is already subscribed to this game.";
        return false;
    }

    _players.push_back(player_ptr);
    return true;
}

bool game_state::draw_card(player* player, std::string &err) {
    if (!is_player_in_game(player)) {
        err = "Server refused to perform draw_card. Player is not part of the game.";
        return false;
    }
    if (!is_allowed_to_play_now(player)) {
        err = "It's not this players turn yet.";
        return false;
    }
    if (_is_finished->get_value()) {
        err = "Could not draw card, because the requested game is already finished.";
        return false;
    }
    if (player->has_drawn()) {
        err = "Drawing a card is permitted at most once per turn.";
        return false;
    }
    /* After checking these conditions drawing a card cannot fail anymore.
     * We already set the has_drawn value to true here, because it is only intended for drawing performed by a player
     * action. However, drawing because of an special card/event card does not affect this rule.
     * To this, those functions call directly void game_state::draw_card_action(player *player, std::string &err)
    */
    _players[_current_player_idx->get_value()]->set_has_drawn(true);
    draw_card_action(player);
    return true;
}

/* In diese Funktion ist die eigentliche Aktion des Ziehens ausgelagert, damit sie entweder im Rahmen einer
 * Spieler-Aktion game_state::draw_card() oder durch ein Ereignis aufgerufen werden kann. Das ist erforderlich, da
 * bei einer Spieleraktion geprüft werden muss, ob der Spieler am Zug ist, bei einem Event aber nicht.
 */

void game_state::draw_card_action(player* player) {
    if (_draw_pile->is_empty()) {
        draw_pile reshuffled_pile = draw_pile(_discard_pile->get_pile_for_reshuffling());
        this->_draw_pile = &reshuffled_pile;
    }
    _draw_pile->draw(player);
}

bool game_state::play_card(player *player, const std::string& card_id, uint new_wish,uint ff_target, std::string &err) {
    if (!is_player_in_game(player)) {
        err = "Server refused to perform play_card. Player is not part of the game.";
        return false;
    }
    if (!is_allowed_to_play_now(player)) {
        err = "It's not this players turn yet.";
        return false;
    }
    if (_is_finished->get_value()) {
        err = "Could not play card, because the requested game is already finished.";
        return false;
    }
    
    // The wish is directly accessed from the attribute.
    if (_discard_pile->try_play(card_id, player,this->get_players(), this->_wish->get_value(), err)) {
        Color topCardColor = _discard_pile->get_top_card()->get_color();
				card* top = const_cast<card*>(_discard_pile->get_top_card());
				CardActionType action = top->get_action();
				if (action == CardActionType::FANTASTIC_FOUR) {
						if (ff_target < _players.size()) {
								for (int draw_count = 0; draw_count < 4; ++draw_count) {
								    _draw_pile->draw(_players[ff_target]);
								}
						}
				} else if (action == CardActionType::EQUALITY)	{
						if (ff_target < _players.size()) {
								int cards_self = player->get_nof_cards();
								int cards_target = _players[ff_target]->get_nof_cards();

								if (cards_target < cards_self)	{
								    int diff = cards_self - cards_target;

								    for (int i = 0; i < diff; ++i) {
								        _draw_pile->draw(_players[ff_target]);
								    }
								}
						}
				} else if(action ==CardActionType::SECOND_CHANCE) {
					return true;
				}
				else if(action==CardActionType::SKIP)	{
						skip_next_player();
				}
				else if(action==CardActionType::REVERSE) {
						reverse_direction();
				}

        // The effect is executed before it is assed whether the game ends.
        if (topCardColor == Color::BLACK) {
            // The next event card ist automatically uncovered.
            // The following antecedent is expected to be always true as it should be ensured that an event card can be always
            // drawn (re-initialization if empty) and always played automatically. The check is to receive error messages for debugging.
            //event_card* uncovered_event_card;
            if (_event_discard_pile->play(_event_draw_pile->remove_top(err), this, err)) {//
                } else {
                err = "Could not play the drawn event card on the event discard pile.";
            }
        }
        if (player->get_nof_cards() == 0) {
            // end of game. Calculate scores. Prepare new round
            wrap_up_round(err);
        }
        // Server side check s.t. the passed wish color is accepted iff a rainbow card is played.
        if (topCardColor == Color::RAINBOW)        {
            // The JSON message could transmit an invalid wish value that would set an invalid wish.
            // For prevention, the wish is set iff the value is valid.
            if (
                new_wish == static_cast<uint>(Color::BLUE) ||
                new_wish == static_cast<uint>(Color::GREEN) ||
                new_wish == static_cast<uint>(Color::RED) ||
                new_wish == static_cast<uint>(Color::YELLOW) ||
                (new_wish > 0 && new_wish <= 9)
                ) {
                this->_wish->set_value(new_wish);
            }
        } else {
            this->_wish->set_value(0);
        }
        update_current_player(err);
        return true;
    }
    return false;
}




bool game_state::fold(player *player, std::string &err) {
    if(!is_player_in_game(player)) {
        err = "Server refused to perform Fold. Player is not part of the game.";
        return false;
    }
    if (!is_allowed_to_play_now(player)) {
        err = "It's not this players turn yet.";
        return false;
    }
    if (_is_finished->get_value()) {
        err = "Could not fold, because the requested game is already finished.";
        return false;
    }
   // Allow next player to play
        update_current_player(err);
        return true;
}
void game_state::set_time_bomb() {
    _final_turn->set_value(_current_turn_idx->get_value() + 3);
}

// end of round functions _has_drawn->set_value(false);
void game_state::update_current_player(std::string& err) {
    int nof_players = _players.size();
    int current_player_idx = _current_player_idx->get_value();
    int current_turn_idx = _current_turn_idx->get_value();
    //Reset has_drawn and pass turn to next player unless the time bomb 'explodes'
    if (_final_turn->get_value() == _current_turn_idx->get_value()) {
        wrap_up_round(err);
    } else {
				_players[current_player_idx]->set_has_drawn(false);
						current_turn_idx += current_player_idx / nof_players;
						_current_turn_idx->set_value(current_turn_idx);

						int dir = _play_direction->get_value();
						if (dir == 0) dir = 1;


						int steps = 1;
						if (_skip_next_player) {
								steps += 1;
								_skip_next_player = false;
						}

						int next_index = current_player_idx + dir * steps;


						next_index %= nof_players;
						if (next_index < 0) {
								next_index += nof_players;
						}

						_current_player_idx->set_value(next_index);
    }
}



void game_state::wrap_up_round(std::string& err) {
    for(int i = 0; i < _players.size(); i++) {
        _players[i]->wrap_up_round(err);
        if (_players[i]->get_score() >= 140) {
            // The game ends when the first player reaches 140 points
            this->_is_finished->set_value(true);
            return;
        }
    }

    // decide which player starts in the next round
    _starting_player_idx->set_value((_starting_player_idx->get_value() + 1) % _players.size());
    // start next round
    setup_round(err);

}

void game_state::end_game() {
    _is_finished->set_value(true);
}

#endif


// Serializable interface
void game_state::write_into_json(rapidjson::Value &json,
                                 rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator> &allocator) const {
    unique_serializable::write_into_json(json, allocator);

    rapidjson::Value is_finished_val(rapidjson::kObjectType);
    _is_finished->write_into_json(is_finished_val, allocator);
    json.AddMember("is_finished", is_finished_val, allocator);

    rapidjson::Value is_started_val(rapidjson::kObjectType);
    _is_started->write_into_json(is_started_val, allocator);
    json.AddMember("is_started", is_started_val, allocator);

    rapidjson::Value current_player_idx_val(rapidjson::kObjectType);
    _current_player_idx->write_into_json(current_player_idx_val, allocator);
    json.AddMember("current_player_idx", current_player_idx_val, allocator);

    rapidjson::Value current_turn_idx_val(rapidjson::kObjectType);
    _current_turn_idx->write_into_json(current_turn_idx_val, allocator);
    json.AddMember("current_turn_idx", current_turn_idx_val, allocator);

    rapidjson::Value play_direction_val(rapidjson::kObjectType);
    _play_direction->write_into_json(play_direction_val, allocator);
    json.AddMember("play_direction", play_direction_val, allocator);

    rapidjson::Value starting_player_idx_val(rapidjson::kObjectType);
    _starting_player_idx->write_into_json(starting_player_idx_val, allocator);
    json.AddMember("starting_player_idx", starting_player_idx_val, allocator);

    rapidjson::Value round_number_val(rapidjson::kObjectType);
    _round_number->write_into_json(round_number_val, allocator);
    json.AddMember("round_number", round_number_val, allocator);

    rapidjson::Value draw_pile_val(rapidjson::kObjectType);
    _draw_pile->write_into_json(draw_pile_val, allocator);
    json.AddMember("draw_pile", draw_pile_val, allocator);

    rapidjson::Value discard_pile_val(rapidjson::kObjectType);
    _discard_pile->write_into_json(discard_pile_val, allocator);
    json.AddMember("discard_pile", discard_pile_val, allocator);

    rapidjson::Value event_draw_pile_val(rapidjson::kObjectType);
    _event_draw_pile->write_into_json(event_draw_pile_val, allocator);
    json.AddMember("event_draw_pile", event_draw_pile_val, allocator);

    rapidjson::Value event_discard_pile_val(rapidjson::kObjectType);
    _event_discard_pile->write_into_json(event_discard_pile_val, allocator);
    json.AddMember("event_discard_pile", event_discard_pile_val, allocator);

    rapidjson::Value wish_val(rapidjson::kObjectType);
    _wish->write_into_json(wish_val, allocator);
    json.AddMember("wish", wish_val, allocator);

    rapidjson::Value final_turn_val(rapidjson::kObjectType);
    _final_turn->write_into_json(final_turn_val, allocator);
    json.AddMember("final_turn", final_turn_val, allocator);

    json.AddMember("players", vector_utils::serialize_vector(_players, allocator), allocator);
}


game_state* game_state::from_json(const rapidjson::Value &json) {
    if (json.HasMember("id")
        && json.HasMember("is_finished")
        && json.HasMember("is_started")
        && json.HasMember("current_player_idx")
        && json.HasMember("current_turn_idx")
        && json.HasMember("play_direction")
        && json.HasMember("round_number")
        && json.HasMember("starting_player_idx")
        && json.HasMember("players")
        && json.HasMember("draw_pile")
        && json.HasMember("discard_pile")
        && json.HasMember("event_draw_pile")
        && json.HasMember("event_discard_pile")
        && json.HasMember("wish")
        && json.HasMember("final_turn")
        )
    {
        std::vector<player*> deserialized_players;
        for (auto &serialized_player : json["players"].GetArray()) {
            deserialized_players.push_back(player::from_json(serialized_player.GetObject()));
        }
        return new game_state(json["id"].GetString(),
                              draw_pile::from_json(json["draw_pile"].GetObject()),
                              discard_pile::from_json(json["discard_pile"].GetObject()),
                              event_draw_pile::from_json(json["event_draw_pile"].GetObject()),
                              event_discard_pile::from_json(json["event_discard_pile"].GetObject()),
                              deserialized_players,
                              serializable_value<bool>::from_json(json["is_started"].GetObject()),
                              serializable_value<bool>::from_json(json["is_finished"].GetObject()),
                              serializable_value<int>::from_json(json["current_player_idx"].GetObject()),
                              serializable_value<int>::from_json(json["current_turn_idx"].GetObject()),
                              serializable_value<int>::from_json(json["play_direction"].GetObject()),
                              serializable_value<int>::from_json(json["round_number"].GetObject()),
                              serializable_value<int>::from_json(json["starting_player_idx"].GetObject()),
                              serializable_value<uint>::from_json(json["wish"].GetObject()),
                              serializable_value<int>::from_json(json["final_turn"].GetObject())
                              );


    } else {
        throw FranticException("Failed to deserialize game_state. Required entries were missing.");
    }
}

