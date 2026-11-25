//
// Created by daniel on 12.12.25.
//

#include "gtest/gtest.h"
#include "../src/common/game_state/game_state.h"
#include "../src/common/game_state/player/player.h"

class PlayerTest : public ::testing::Test {
  protected:
  virtual void SetUp() {
    sCards.resize(5);
    for (CardActionType i = CardActionType::SKIP; i != CardActionType::END; i = static_cast<CardActionType>(static_cast<uint>(i) + 1)) {
      for (Color j = Color::BLUE; j != Color::RAINBOW; j = static_cast<Color>(static_cast<uint>(j) + 1)) {
        sCards[static_cast<uint>(i) - 1].push_back(new card(j, i));
        }
    }
    player* p0 = new player("Elizabeth");
    player* p1 = new player("Philipp");
    player* p2 = new player("Charles");
    player* p3 = new player("Camilla");
    player* p4 = new player("Catherine");
    player* p5 = new player("William");
    gs.add_player(p0, err);
    gs.add_player(p1,  err);
    gs.add_player(p2,  err);
    gs.add_player(p3,  err);
    gs.add_player(p4,  err);
    gs.add_player(p5,  err);
    gs.start_game(err);
    topCol = gs.get_discard_pile()->get_top_card()->get_color();
    topVal = gs.get_discard_pile()->get_top_card()->get_value();
    len = gs.get_players().size();
  }

  game_state gs;
  std::vector<player*> players;
  std::string err;
  // 0: SKIP, 1: REVERSE, 2: FANTASTIC_FOUR, 3: EQUALITY, 4: SECOND_CHANCE
  std::vector<std::vector<card*>> sCards;
  Color topCol;
  uint topVal;
  uint len;
};

// MT06: Verify that a player can draw a card from the deck and that the hand size increases accordingly
TEST_F(PlayerTest, DrawCard) {
  player* aP = gs.get_current_player();
ASSERT_EQ(aP->get_nof_cards(), 6);
gs.draw_card(aP, err);
 EXPECT_EQ(aP->get_nof_cards(), 7);
}

// Test reverse Direction function
TEST_F(PlayerTest, ReverseDirection) {
  int initDir = 1;
  EXPECT_EQ(gs.get_play_direction(), initDir);
  gs.reverse_direction();
  EXPECT_EQ(gs.get_play_direction(), initDir * (-1));
}

// MT08: Reverse Card alters turn direction
TEST_F(PlayerTest, ReverseCard) {
  player* aP = gs.get_current_player();
  uint idx = gs.get_current_player_idx();
  Color altCol = Color::GREEN;
  if (topCol == Color::BLACK) { // An intermediate card is required as there is no black version of special cards
    topCol = altCol;
    card* intermediateCard = new card(topVal, topCol);
    aP->add_card(intermediateCard);
    gs.play_card(aP, intermediateCard->get_id(), 0, 0, err);
    gs.set_current_player_idx(idx);
  }
  if (topCol == Color::RAINBOW) {
    topCol = altCol; // Any card can be played upon a rainbow card
  }
  card* revCard = sCards[1][static_cast<uint>(topCol) - 11];
  std::string id = revCard->get_id();
  int initDir = 1;
  EXPECT_EQ(gs.get_play_direction(), initDir);
  aP->add_card(revCard);
  gs.play_card(aP, id, 0, 0, err);
  EXPECT_EQ(gs.get_play_direction(), initDir * (-1));
}

// Test Skip next player Function
TEST_F(PlayerTest, SkipFunction) {

  for (int i = 0; i < len; ++i) {
    gs.set_current_player_idx(i);
    uint idx = gs.get_current_player_idx();
    gs.skip_next_player();
    gs.update_current_player(err);
    EXPECT_EQ(gs.get_current_player_idx(), (idx + 2) % len);
  }
}

// MT09: Skip Card skips exactly one player
TEST_F(PlayerTest, SkipCard) {
  player* aP = gs.get_current_player();
  uint idx = gs.get_current_player_idx();
  Color altCol = Color::GREEN;
  if (topCol == Color::BLACK) { // An intermediate card is required as there is no black version of special cards
    topCol = altCol;
    card* intermediateCard = new card(topVal, topCol);
    aP->add_card(intermediateCard);
    gs.play_card(aP, intermediateCard->get_id(), 0, 0, err);
    gs.set_current_player_idx(idx);
  }
  if (topCol == Color::RAINBOW) {
    topCol = altCol; // Any card can be played upon a rainbow card
  }
  card* skipCard = sCards[0][static_cast<uint>(topCol) - 11];
  aP->add_card(skipCard);
  gs.play_card(aP, skipCard->get_id(), 0, 0, err);
  ASSERT_EQ(gs.get_current_player_idx(), (idx + 2) % len);
}

// ST03: Verify correct initial card distribution
TEST_F(PlayerTest, CardDistribution) {
  for (int i = 0; i<len; ++i) {
    EXPECT_EQ(gs.get_players()[i]->get_nof_cards(), 6);
  }
}

// ST03: Verify initially empty event discard pile
TEST_F(PlayerTest, EventDiscardPileEmpty) {
    EXPECT_EQ(gs.get_event_discard_pile()->get_top_card(), nullptr);
}

// ST04: Illegal move is rejected and state remains unchanged
TEST(cardTest, IllegalMove) {
  card* c1 = new card(1, Color::BLUE);
  card* c2 = new card(2, Color::RED);
  card* c3 = new card(2, Color::GREEN);

  player* p1 = new player("Alice");
  player* p2 = new player("Elizabeth");
  player* p3 = new player("John");
  game_state* gs = new game_state();

#ifdef FRANTIC_SERVER
  p1->add_card(c1);
  p2->add_card(c2);
  p3->add_card(c3);

  std::string err;
  gs->add_player(p1, err);
  gs->add_player(p2, err);
  gs->add_player(p3, err);
  gs->set_current_player_idx(0);

  card* top_discard = new card(3, Color::RED);
  discard_pile dp;
  dp.addCard(top_discard, err);
  gs->set_discard_pile(&dp);

  uint ff_target = 0;
  uint new_wish = static_cast<uint>(Color::GREEN);

  //for comparing old and new game state
  auto old_top_discard = gs->get_discard_pile()->get_top_card()->get_id();
  uint old_wish = gs->get_wish();
  int old_current_player = gs->get_current_player_idx();

  std::vector<std::vector<std::string>> old_hands;
  for (auto* p : gs->get_players()) {
    std::vector<std::string> hand_ids;
    for (auto* c : p->get_hand()->get_cards()) {
      hand_ids.push_back(c->get_id());
    }
    old_hands.push_back(hand_ids);
  }

  //illegal move
  EXPECT_FALSE(gs->play_card(p1, c1->get_id(), new_wish, ff_target, err));

  //game state should not have changed
  //check discard pile, wish, and current player unchanged
  EXPECT_EQ(old_top_discard, gs->get_discard_pile()->get_top_card()->get_id());
  EXPECT_EQ(old_wish, gs->get_wish());
  EXPECT_EQ(old_current_player, gs->get_current_player_idx());

  //check that hands are unchanged
  auto players = gs->get_players();
  for (size_t i = 0; i < players.size(); ++i) {
    auto cards = players[i]->get_hand()->get_cards();
    ASSERT_EQ(old_hands[i].size(), cards.size());
    for (size_t j = 0; j < cards.size(); ++j) {
      EXPECT_EQ(old_hands[i][j], cards[j]->get_id());
    }
  }

#endif
}


// Game ends when a player plays his last card

TEST_F(PlayerTest, PlayFinalCard) {
  int pInd = 2;
  player* p = gs.get_players()[pInd];
  hand* h = p->get_hand();
  std::vector<card*> cards = h->get_cards();
  for (int i = 0; i<cards.size(); ++i) {
    h->remove_card(cards[i]->get_id(), cards[i], err);
  }
  EXPECT_EQ(p->get_nof_cards(), 0);
  p->add_card(new card(topVal, topCol));
  gs.set_current_player_idx(pInd);
  EXPECT_TRUE(gs.play_card(p, h->get_cards()[0]->get_id(), 0, 0, err));
  EXPECT_EQ(p->get_nof_cards(), 6);
}

//ST05
TEST_F(PlayerTest, AddScores) {
    std::vector<int> cS;
    std::vector<int> nS;

    for (auto* player: gs.get_players()) {
        cS.push_back(player->get_score());
        nS.push_back(player->get_hand()->get_score());
    }
    gs.wrap_up_round(err);
    for (size_t i = 0; i < gs.get_players().size(); ++i) {
        EXPECT_EQ(gs.get_players()[i]->get_score(), cS[i] + nS[i]);
    }
}
//ST05
TEST_F(PlayerTest, RoundEndsGameContinueNewRound) {
    ASSERT_FALSE(gs.is_finished());

    int rounds = gs.get_round_number();
    gs.wrap_up_round(err);
    EXPECT_EQ(gs.get_round_number(), rounds + 1);

    for (auto* player : gs.get_players()) {
        EXPECT_EQ(player->get_nof_cards(), 6);
    }

    EXPECT_EQ(gs.get_event_discard_pile()->get_top_card(), nullptr);
    ASSERT_NE(gs.get_discard_pile()->get_top_card(), nullptr);
}
//ST05
TEST_F(PlayerTest, GameEnds) {
    int pInd = 2;
    player* p = gs.get_players()[pInd];

    p->add_score(139);
    p->add_card(new card(1,Color::BLUE));

    ASSERT_FALSE(gs.is_finished());
    gs.wrap_up_round(err);
    EXPECT_TRUE(gs.is_finished());


}