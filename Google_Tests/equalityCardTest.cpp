#include "gtest/gtest.h"

#include "../src/common/game_state/game_state.h"
#include "../src/common/game_state/player/player.h"
#include "../src/common/game_state/cards/card.h"
#include "../src/common/game_state/cards/card_action_type.h"

//MT10; due to changes in the Equalitycardpanel, it is no longer possible to choose
// an invalid target. Here we test the playability of the card (can only be played if there is at
//least one valid target, and the correct execution of the effect (target draws up to attakers handsize
//after playing)

namespace {

//create a normal number card 1 to9 with a chosen color.
static card* mk(uint v, Color c) { return new card(v, c); }

static void add_n_cards(player* p, int n) {
    const Color colors[] = {Color::BLUE, Color::GREEN, Color::RED, Color::YELLOW};
    for (int i = 0; i < n; ++i) {
        p->add_card(mk(static_cast<uint>((i % 9) + 1), colors[i % 4]));
    }
}

}


class EqualityCardTest : public ::testing::Test {
protected:
    void SetUp() override {
        gs = new game_state();
        p0 = new player("Attacker");
        p1 = new player("Target");
        p2 = new player("Other");
				
        std::string err;
        ASSERT_TRUE(gs->add_player(p0, err)) << err;
        ASSERT_TRUE(gs->add_player(p1, err)) << err;
        ASSERT_TRUE(gs->add_player(p2, err)) << err;
        ASSERT_TRUE(gs->start_game(err)) << err;
        gs->set_current_player_idx(0);
    }

    void TearDown() override {
        delete p0;
        delete p1;
        delete p2;
        delete gs;
        p0 = nullptr;
        p1 = nullptr;
        p2 = nullptr;
        gs = nullptr;
    }

    void clear_hands() {
        std::string err;
        p0->get_hand()->setup_round(err);
        p1->get_hand()->setup_round(err);
        p2->get_hand()->setup_round(err);
    }

    game_state* gs = nullptr;
    player* p0 = nullptr;
    player* p1 = nullptr;
    player* p2 = nullptr;
};


// The EQUALITY card may only be played if, after playing there exists at least
// one other player with strictly fewer cards than the current player.
// After selecting such a player, that target must draw cards until it has as many
// cards as the attacker.
TEST_F(EqualityCardTest, CanPlayAndTargetDrawsUpToAttackerHandSize) {
    clear_hands();
		
    // Attacker has 5 cards incl. the equality card.
    card* eq = new card(Color::RAINBOW, CardActionType::EQUALITY);
    p0->add_card(eq);
    add_n_cards(p0, 4);
		
    // Target has 2 cards, so playing EQUALITY is ok.
    add_n_cards(p1, 2);
    // Another player with more cards.
    add_n_cards(p2, 6);
		
    const int before_attacker = p0->get_nof_cards();
    const int before_target = p1->get_nof_cards();
		
    std::string err;
    const uint new_wish = 0;
    const uint target_idx = 1; // p1
    ASSERT_TRUE(gs->play_card(p0, eq->get_id(), new_wish, target_idx, err)) << err;
		
    // attacker spent one card
    EXPECT_EQ(before_attacker - 1, p0->get_nof_cards());
		
    // target draws up to attackers current size
    EXPECT_EQ(p0->get_nof_cards(), p1->get_nof_cards());
    EXPECT_EQ(p0->get_nof_cards(), before_target + (before_attacker - 1 - before_target));
		
    // top card is the equality card
    const card* top = gs->get_discard_pile()->get_top_card();
    ASSERT_NE(nullptr, top);
    EXPECT_EQ(CardActionType::EQUALITY, top->get_action());
}


TEST_F(EqualityCardTest, RejectsPlayIfNobodyHasFewerCardsAfterPlaying) {
    clear_hands();
		
    // Attacker has 5 cards incl. equality; after playing  4.
    card* eq = new card(Color::RAINBOW, CardActionType::EQUALITY);
    p0->add_card(eq);
    add_n_cards(p0, 4);
		
    // Others each have 4 cards,  nobody has < 4.
    add_n_cards(p1, 4);
    add_n_cards(p2, 4);
		
    const int before_attacker = p0->get_nof_cards();
    const card* top_before = gs->get_discard_pile()->get_top_card();
    ASSERT_NE(nullptr, top_before);
    const std::string top_before_id = top_before->get_id();
		
    std::string err;
    EXPECT_FALSE(gs->play_card(p0, eq->get_id(), 0, 0, err));
    EXPECT_EQ("Equality card cannot be played because the current player already has the fewest cards.", err);
		
    // cards remains in hand, discard pile unchanged
    EXPECT_EQ(before_attacker, p0->get_nof_cards());
    const card* top_after = gs->get_discard_pile()->get_top_card();
    ASSERT_NE(nullptr, top_after);
    EXPECT_EQ(top_before_id, top_after->get_id());
}
