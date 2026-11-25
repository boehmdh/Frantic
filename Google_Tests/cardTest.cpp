#include "gtest/gtest.h"
#include "../src/common/exceptions/FranticException.h"
#include "../src/common/game_state/cards/card.h"
#include "../src/common/serialization/json_utils.h"
#include "../src/common/game_state/player/player.h"
#include "../src/common/game_state/game_state.h"
#include "../src/common/game_state/cards/discard_pile.h"


// A card can be played on another card if and only if the new card has the
// same value or color as the top card on the discard pile (except if a wish is active)
// a wish card may always be played (MTO1)
TEST(CardTest, PlayValidCard) {
    //standard cards
    card c1(1, Color::BLUE);
    card c2(2, Color::RED);
    card c3(1, Color::RED);   //same value as c1, different color
    card c4(4, Color::BLUE);  //same color as c1, different value
    card c5(5, Color::YELLOW);

    //Black cards
    card b1(1, Color::BLACK);
    card b2(2, Color::BLACK);

    //Rainbow card (wish)
    card rainbow(20, Color::RAINBOW); //

    //standard rules
    EXPECT_TRUE(c1.can_be_played_on(&c4)); //same color
    EXPECT_TRUE(c1.can_be_played_on(&c3)); //same value

    //black card rules
    EXPECT_TRUE(b1.can_be_played_on(&b1)); //same value
    EXPECT_TRUE(b1.can_be_played_on(&c1)); //same value
    EXPECT_TRUE(c1.can_be_played_on(&b1));
	EXPECT_FALSE(b1.can_be_played_on(&b2)); //different value, but same color black

    // Rainbow card can be played on any card including black
    EXPECT_TRUE(rainbow.can_be_played_on(&c1));
    EXPECT_TRUE(rainbow.can_be_played_on(&c2));
	EXPECT_TRUE(rainbow.can_be_played_on(&b2));
}
//MT02
TEST(CardTest, PlayInvalidCard) {
    //standard cards
    card c1(1, Color::BLUE);
    card c2(2, Color::RED);
    card c3(1, Color::RED);   //same value as c1, different color
    card c4(4, Color::BLUE);  //same color as c1, different value
    card c5(5, Color::YELLOW);

    //Black cards
    card b1(1, Color::BLACK);
    card b2(2, Color::BLACK);

    //Rainbow card (wish)
    card rainbow(0, Color::RAINBOW); //Is it 0?

    //standard rules
    EXPECT_FALSE(c1.can_be_played_on(&c2)); //different value and color
    EXPECT_FALSE(c1.can_be_played_on(&c5));

    //black card rules
    EXPECT_FALSE(b1.can_be_played_on(&b2)); //different value


}
//MT03
TEST(CardTest, PlayRainbowCard) {
	card* rainbow1 = new card(20, Color::RAINBOW);
	card* rainbow2 = new card(20, Color::RAINBOW);
    card* c1 = new card(1, Color::BLUE);
    card* c2 = new card (2, Color::RED);
    card* c3 = new card(1, Color::RED);
    card* c4 = new card(4, Color::BLUE);
    card* c5 = new card(5, Color::YELLOW);

	card* b1 = new card(1, Color::BLACK);

    EXPECT_TRUE(rainbow1->can_be_played_on(c1));
    EXPECT_TRUE(rainbow1->can_be_played_on(c2));
	EXPECT_TRUE(rainbow1->can_be_played_on(c3));
	EXPECT_TRUE(rainbow1->can_be_played_on(c4));
	EXPECT_TRUE(rainbow1->can_be_played_on(c5));
	EXPECT_TRUE(rainbow1->can_be_played_on(b1));
	EXPECT_TRUE(rainbow1->can_be_played_on(rainbow2));

	//play rainbow card and check if top of discard pile color is updated
	player* p1 = new player("Alice");
	player* p2 = new player("Elizabeth");
	game_state* gs = new game_state();

	#ifdef FRANTIC_SERVER
	p1->add_card(rainbow1);
	p1->add_card(rainbow2);

	std::string err;
	gs->add_player(p1, err);
	gs->add_player(p2, err);
	gs->set_current_player_idx(0);

	uint ff_target = 0;
	uint new_wish = static_cast<uint>(Color::RED);

	// MT03: ensure a valid color wish will be set
	EXPECT_TRUE(gs->play_card(p1, rainbow1->get_id(), new_wish, ff_target, err));
	EXPECT_TRUE(gs->get_wish() == static_cast<uint>(Color::RED));
	gs->set_wish(0);

	// Ensure a valid number wish will be set
	gs->set_current_player_idx(0);
	uint numWish = 5;
	EXPECT_TRUE(gs->play_card(p1, rainbow2->get_id(), numWish, ff_target, err));
	EXPECT_TRUE(gs->get_wish() == numWish);

	#endif

}
// MT04: Send an invalid wish with playing a rainbow card -> ensure no wish is set
TEST(CardTest, MT04) {
	card* rainbow = new card(20, Color::RAINBOW);
	player* p1 = new player("Alice");
	player* p2 = new player("Elizabeth");
	game_state* gs = new game_state();

#ifdef FRANTIC_SERVER
	p1->add_card(rainbow);


	std::string err;
	gs->add_player(p1, err);
	gs->add_player(p2, err);
	gs->set_current_player_idx(0);


	EXPECT_TRUE(gs->play_card(p1, rainbow->get_id(), static_cast<uint>(Color::BLACK), 0, err));
	EXPECT_TRUE(gs->get_wish() == 0);

#endif
}

// MT05: Send a wish without playing a rainbow card -> ensure no wish is set
TEST(CardTest, MT05) {
	card* c1 = new card(1, Color::BLUE);
	player* p1 = new player("Alice");
	player* p2 = new player("Elizabeth");
	game_state* gs = new game_state();

#ifdef FRANTIC_SERVER
	p1->add_card(c1);


	std::string err;
	gs->add_player(p1, err);
	gs->add_player(p2, err);
	gs->set_current_player_idx(0);


	EXPECT_TRUE(gs->play_card(p1, c1->get_id(), static_cast<uint>(Color::BLUE), 0, err));
	EXPECT_TRUE(gs->get_wish() == 0);

#endif
}


// Serialization and subsequent deserialization must yield the same object
TEST(CardTest, SerializationEquality) {
    card card_send(1, Color::GREEN);
    rapidjson::Document* json_send = card_send.to_json();
    std::string message = json_utils::to_string(json_send);
    delete json_send;

    rapidjson::Document json_recv = rapidjson::Document(rapidjson::kObjectType);
    json_recv.Parse(message.c_str());
    card* card_recv = card::from_json(json_recv);
    EXPECT_EQ(card_send.get_id(), card_recv->get_id());
    EXPECT_EQ(card_send.get_value(), card_recv->get_value());
    delete card_recv;
}

// Deserializing an invalid string must throw a FranticException
TEST(CardTest, SerializationException) {
    rapidjson::Document json = rapidjson::Document(rapidjson::kObjectType);
    json.Parse("not json");
    EXPECT_THROW(card::from_json(json), FranticException);
}

