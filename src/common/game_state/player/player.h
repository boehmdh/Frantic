//
// Created by Manuel on 25.01.2021.
//

#ifndef FRANTIC_PLAYER_H
#define FRANTIC_PLAYER_H


#include <string>
#include "hand.h"
#include "../../serialization/uuid_generator.h"
#include "../../../../rapidjson/include/rapidjson/document.h"
#include "../../serialization/unique_serializable.h"
#include "../../serialization/serializable_value.h"
/// For every client a player object is treated that holds its hand, score and information whether the player has already drawn a card, which allows them to fold.
class player : public unique_serializable {
private:
    serializable_value<std::string>* _player_name;
    serializable_value<bool>* _has_drawn;
    serializable_value<int>* _score;
    hand* _hand;

#ifdef FRANTIC_SERVER
    std::string _game_id;
#endif

    /*
     * Deserialization constructor
     */
    player(std::string id,
           serializable_value<std::string>* name,
           serializable_value<int>* score,
           hand* hand,
           serializable_value<bool>* has_drawn);

public:
// constructors
    explicit player(std::string name);   // for client
    ~player();

#ifdef FRANTIC_SERVER
    player(std::string id, std::string name);  // for server

    std::string get_game_id();
    void set_game_id(std::string game_id);
#endif

    // accessors
    /// To determine whether a player has exceeded the limit of 140 Points that ends the game.
    int get_score() const noexcept;
    //! Returns true if the player has already drawn in this round.

    //! Is only set to true by a deliberately drawing a card, not if an effect or a 'Fantastic Four' card forces the
    //! player to draw. Only if the player has drawn a card, he is permitted to fold.
    bool has_drawn() const noexcept;
    int get_nof_cards() const noexcept;
    hand* get_hand() const noexcept;
    std::string get_player_name() const noexcept;

#ifdef FRANTIC_SERVER
    // state update functions
    void add_card(card* card);
    bool remove_card(std::string card_id, card*& card, std::string& err);
    //! Folds and returns true if permitted.

    //! Checks whether the player is on turn and has already drawn a card. If both is true, the turn is passed to the next
    //! player. Otherwise no effect takes place and the method returns false.
    bool fold(std::string &err);
    void wrap_up_round(std::string& err);
    void setup_round(std::string& err);
    /// Setter for testing.
    void set_has_drawn(bool has_drawn);
    /// Add a score by the game state at the end of each round. Required for the server-side check.
    void add_score(int amount);
#endif


    // serialization
    static player* from_json(const rapidjson::Value& json);
    virtual void write_into_json(rapidjson::Value& json, rapidjson::Document::AllocatorType& allocator) const override;

};


#endif //FRANTIC_PLAYER_H
