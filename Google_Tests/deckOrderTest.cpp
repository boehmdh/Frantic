#include "gtest/gtest.h"

#include "../src/common/game_state/game_state.h"
#include "../src/common/game_state/player/player.h"
#include "../src/common/game_state/cards/draw_pile.h"
#include "../src/common/game_state/cards/card.h"

#include <tuple>
#include <vector>

static std::tuple<uint, uint, uint> sig(const card* c) {
    return {
        c->get_value(),
        static_cast<uint>(c->get_color()),
        static_cast<uint>(c->get_action())
    };
}

static void append_hand_signature(std::vector<std::tuple<uint, uint, uint>>& out, const player* p) {
    const auto cards = p->get_hand()->get_cards(); // copy of ptrvector
    for (const card* c : cards) {
        out.push_back(sig(c));
    }
}

static void append_top_draws_signature(std::vector<std::tuple<uint, uint, uint>>& out,
                                      draw_pile* pile,
                                      int n,
                                      std::vector<card*>& to_delete,
                                      std::string& err) {
    for (int i = 0; i < n; ++i) {
        card* c = pile->remove_top(err);
        ASSERT_NE(c, nullptr) << "remove_top failed: " << err;
        out.push_back(sig(c));
        to_delete.push_back(c); //removed cards are no longer owned by draw_pile
    }
}

TEST(GameStateTest, MT07_RandomizedDeckOrder) {
#ifdef FRANTIC_SERVER
    std::string err;

    // --- Game instance 1 ---
    game_state* gs1 = new game_state();
    player* a1 = new player("A");
    player* b1 = new player("B");

    ASSERT_TRUE(gs1->add_player(a1, err)) << err;
    ASSERT_TRUE(gs1->add_player(b1, err)) << err;
    ASSERT_TRUE(gs1->start_game(err)) << err;

    std::vector<std::tuple<uint, uint, uint>> seq1;
    append_hand_signature(seq1, a1);
    append_hand_signature(seq1, b1);

    std::vector<card*> drawn1;
    append_top_draws_signature(seq1, gs1->get_draw_pile(), 3, drawn1, err);

    // --- Game instance 2 ---
    game_state* gs2 = new game_state();
    player* a2 = new player("A");
    player* b2 = new player("B");

    ASSERT_TRUE(gs2->add_player(a2, err)) << err;
    ASSERT_TRUE(gs2->add_player(b2, err)) << err;
    ASSERT_TRUE(gs2->start_game(err)) << err;

    std::vector<std::tuple<uint, uint, uint>> seq2;
    append_hand_signature(seq2, a2);
    append_hand_signature(seq2, b2);

    std::vector<card*> drawn2;
    append_top_draws_signature(seq2, gs2->get_draw_pile(), 3, drawn2, err);

    // same shape (2 hands + 3 draws), but should differ at least once
    ASSERT_EQ(seq1.size(), seq2.size());

    bool differs = false;
    for (size_t i = 0; i < seq1.size(); ++i) {
        if (seq1[i] != seq2[i]) {
            differs = true;
            break;
        }
    }
    EXPECT_TRUE(differs) << "MT07 failed: deck order appears deterministic (identical sequences).";

    // cleanup
    for (card* c : drawn1) delete c;
    for (card* c : drawn2) delete c;

    delete gs1;
    delete gs2;

    // game_state does NOT own players ,delete them
    delete a1; delete b1;
    delete a2; delete b2;
#endif
}

