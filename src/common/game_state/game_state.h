//
//
//

#ifndef FRANTIC_GAME_STATE_H
#define FRANTIC_GAME_STATE_H

#include <vector>
#include <string>
#include "../../rapidjson/include/rapidjson/document.h"
#include "player/player.h"
#include "cards/draw_pile.h"
#include "cards/discard_pile.h"
#include "cards/event_draw_pile.h"
#include "cards/event_discard_pile.h"
#include "../serialization/serializable.h"
#include "../serialization/serializable_value.h"
#include "../serialization/unique_serializable.h"

/**
 * This is the main class comprising the current game state and functions for any game action.
 *
 * As this class' field holds the game state, every player action involves invocation of one of the following methods.
 * These comprise a check whether the respective action is valid and reject invalid requests.
 * To facilitate serialization of attributes holding the game state, this class inherits from unique_serializable.
 */
class game_state : public unique_serializable {
private:

    static const int _max_nof_players = 6;
    static const int _min_nof_players = 2;
		bool _skip_next_player = false;
    draw_pile* _draw_pile;
    discard_pile* _discard_pile;
    event_draw_pile* _event_draw_pile;
    event_discard_pile* _event_discard_pile;
    std::vector<player*> _players;
    serializable_value<bool>* _is_started;
    serializable_value<bool>* _is_finished;
    serializable_value<int>* _round_number;
    serializable_value<int>* _current_player_idx;
    serializable_value<int>* _current_turn_idx; // turn counter, zero-based
    serializable_value<int>* _play_direction;  // 1 or -1 depending on which direction is played in
    serializable_value<int>* _starting_player_idx;
    serializable_value<uint>* _wish;
    serializable_value<int>* _final_turn; //for Time bomb, equals -1 unless set


    /// from_diff constructor
    game_state(std::string id);

    /// deserialization constructor
    game_state(
            std::string id,
            draw_pile* draw_pile,
            discard_pile* discard_pile,
            event_draw_pile* event_draw_pile,
            event_discard_pile* event_discard_pile,
            std::vector<player*>& players,
            serializable_value<bool>* is_started,
            serializable_value<bool>* is_finished,
            serializable_value<int>* current_player_idx,
            serializable_value<int>* _current_turn_idx,
            serializable_value<int>* play_direction,
            serializable_value<int>* round_number,

            serializable_value<int>* starting_player_idx,
            serializable_value<uint>* wish,
            serializable_value<int>* final_turn

            );

    /// Returns the index of 'player' in the '_players' vector.
    int get_player_index(player* player) const;
public:
	/// Public constructor.
    game_state();
	/// Public destructor.
    ~game_state();

// accessors
	/// Returns true if the maximum number of players have connected to the game.
    bool is_full() const;
	/// Returns true if the round has already started. According to the rules, joining the game is only allowed if the round has not started yet.
    bool is_started() const;
	/// Returns true if the game has finished. This occures after one player has exceeded the maximum of 140 points.
    bool is_finished() const;
	/// Returns whether the player corresponding to the passed player pointer is part of the game.
    bool is_player_in_game(player* player) const;
	/// Returns whether it is the passed player's turn.
    bool is_allowed_to_play_now(player* player) const;
	/// Returns a list of all players in default turn order. This order is unchanged if the play direction is inverted.
    std::vector<player*>& get_players();
	/// Returns the turn index (zero-based). Required for the special card 'Time bomb'. Incremented whenever
	/// as many players as are in the game have completed their turn. A player's turn affected by the SKIP effect
	/// card is nevertheless counted as a completed turn.
    int get_current_turn_idx() const;
	/// Returns either 1 (anticlockwise, default) or -1 (clockwise).
	int get_play_direction() const;
	/// Returns the round index (zero-based). A round is defined as the interval from setting up the game until one player plays the last card.
    int get_round_number() const;
	/// Returns the index in std::vector<player*> of the player on turn.
	uint get_current_player_idx() const;

	/// Returns a pointer to the draw pile.
    draw_pile* get_draw_pile() const;
	/// Returns a pointer to the discard pile.
    discard_pile* get_discard_pile() const;
	/// Returns a pointer to the event draw pile.
    event_draw_pile* get_event_draw_pile() const;
	/// Returns a pointer to the event discard pile.
    event_discard_pile* get_event_discard_pile() const;
	/// Returns a pointer to the player on turn.
	player* get_current_player() const;
    /// Returns the current wish set by a Rainbow card. Either 1--9 for a number wish or 11--14 for a color wish;
    /// 11: blue, 12: green, 13: red, 14: yellow
    /// A value of 0 informs that there is currently no active wish.
    uint get_wish() const;
	/// Function required for the special card 'TIME BOMB'. Returns -1 unless the 'Time bomb' is activated. Then the index of the final turn will be returned.
    int get_final_turn() const;

	// setters for testing
	/// Sets the draw pile (for unit testing).
	void set_draw_pile(draw_pile* draw);
	/// Sets the discard pile (for unit testing).
	void set_discard_pile(discard_pile* discard);
	/// Sets the current player index (for unit testing).
	void set_current_player_idx(int idx);
	/// Sets the current wish (for unit testing).
	void set_wish(uint value) { _wish->set_value(value); }

	// Functions for special cards
	/// Inverts the play direction (if the 'EXCHANGE'/'REVERSE' card is played.
	void reverse_direction();
	/// Skips the next player on turn (if the 'SKIP' card is played).
    void skip_next_player();
#ifdef FRANTIC_SERVER
// server-side state update functions

	/// Initializes the game: Creates draw piles and event draw piles, distributes cards to the players and transfers one
	/// card to the discard pile. If this card has Color BLACK no event card is uncovered (unlike it was played by a client).
    void setup_round(std::string& err);   // server side initialization
	/// Returns true if the game could be started. Returns false and an according message in err if the game has already
	/// started or only one player joined the game.
	bool start_game(std::string& err);
	/// Removes a player from the game and reduces current_player_idx accordingly.
	bool remove_player(player* player, std::string& err);
	/// Adds a player to the game and returns true. Returns false, if the game has already started/finished or
	/// the maximum number of players has already been reached.
    bool add_player(player* player, std::string& err);
	//! Returns true and draws a card if the player is allowed to do so.

	//! This function checks whether the player is part of the game, it is his turn, the game has not finished and
	//! the player has already drawn a card in this turn. If one of these predicates apply the function returns false.
	//! Otherwise, by calling draw_card_action(player* player) a card is drawn and the function returns true.
    bool draw_card(player* player, std::string& err);
	//! Performs the action of drawing a card.

	//! Checks whether the draw pile is empty before drawing. If this applies, all cards from the discard pile except the
	//! top one are transferred to the draw pile and reshuffled.
    void draw_card_action(player *player);
	//! Returns true if the player could play the card, otherwise false.

	//! Again, it is ensured the player is part of the game, it is his turn, the game has not finished yet and
	//! the Frantic rules allow the card to be played on the top card on the discard pile.
	//! If one of these conditions fail, the method returns false.
	//! Otherwise, the card is played and the respective effects are executed. If a black card is played,
	//! the top card of the event draw pile is played on the event discard pile and the respective effect takes place.
	//! If the player just played his final card, the round is wrapped up.
    bool play_card(player* player, const std::string& card_id, uint new_wish, uint ff_target, std::string& err);
    //! Passes the turn to the next player and returns true if the player is allowed to fold, otherwise false.

	//! Again, it is ensured the player is part of the game, it is his turn, the game has not finished yet.
	//! Moreover, it is checks that the player has already drawn a card in this turn. If all of these conditions apply,
	//! the update_current_player(err) is called and the method returns true.
    bool fold(player* player, std::string& err);
    //! Sets the time bomb to end the current round in three turns.

	//! Only called if the event card 'TIME_BOMB' is uncovered. Sets the final_turn variable s.t. the game ends
	//! automatically after three turns have elapsed.
    void set_time_bomb();

    //! Ends the turn of the player currently on turn.

	//! If the current_turn_idx equals the final_turn_idx (i.e. the time bomb 'explodes'), the current round will be terminated.
	//! Also increments the current_turn_idx variable.
    void update_current_player(std::string& err);
	//! Called upon termination of one round.

	//! Upon the end of a round, increments the player's points according to their remaining hand.
	//! If one player exceeds the limit of 140 points after incrementing, end_game() is called and the game will be
	//! terminated immediately. Otherwise, the subsequent round will be set up.
    void wrap_up_round(std::string& err);
	/// Ends the game after a player has exceeded the limit of 140 points.
    void end_game();



#endif

// serializable interface
	/// For deserialization.
    static game_state* from_json(const rapidjson::Value& json);
	/// For serialization.
    virtual void write_into_json(rapidjson::Value& json, rapidjson::Document::AllocatorType& allocator) const override;

};


#endif //FRANTIC_GAME_STATE_H
