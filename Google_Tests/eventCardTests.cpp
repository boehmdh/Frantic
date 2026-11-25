#include "gtest/gtest.h"

#include "../src/common/exceptions/FranticException.h"
#include "../src/common/game_state/cards/event_card.h"
#include "../src/common/game_state/cards/event_card_collection.h"
#include "../src/common/game_state/game_state.h"
#include "../src/common/game_state/player/player.h"
#include "../src/common/serialization/json_utils.h"

namespace {

std::vector<Event_card_collection> all_event_types() {
    std::vector<Event_card_collection> types;
    for (uint i = 0; i < static_cast<uint>(Event_card_collection::END); ++i) {
        types.push_back(static_cast<Event_card_collection>(i));
    }
    return types;
}

}



// Serialization / Deserialization

class EventCardSerializationTest : public ::testing::TestWithParam<Event_card_collection> {};

TEST_P(EventCardSerializationTest, SerializationEquality) {
    const Event_card_collection typ = GetParam();
		
    event_card send(typ);
    rapidjson::Document* json_send = send.to_json();
    const std::string message = json_utils::to_string(json_send);
    delete json_send;
		
    rapidjson::Document json_recv(rapidjson::kObjectType);
    json_recv.Parse(message.c_str());
    event_card* recv = event_card::from_json(json_recv);
		
    EXPECT_EQ(send.get_id(), recv->get_id());
    EXPECT_EQ(send.get_event_name(), recv->get_event_name());
		
    delete recv;
}

INSTANTIATE_TEST_SUITE_P(AllEventCards,EventCardSerializationTest,::testing::ValuesIn(all_event_types()));


TEST(EventCardSerializationTest, SerializationExceptionOnMissingMembers) {
    rapidjson::Document json(rapidjson::kObjectType);
		
    EXPECT_THROW(event_card::from_json(json), FranticException);
}



// Effects

class EventCardEffectTest : public ::testing::Test {
protected:
    void SetUp() override {
        gs = new game_state();
        p0 = new player("Alice");
        p1 = new player("Bob");
        p2 = new player("Charlie");
				
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
        gs = nullptr;
        p0 = nullptr;
        p1 = nullptr;
        p2 = nullptr;
    }
		
    void clear_hands() {
        std::string err;
        p0->get_hand()->setup_round(err);
        p1->get_hand()->setup_round(err);
        p2->get_hand()->setup_round(err);
    }
		
    //create a normal number card (value 1 to 9) with a distinct color.
    static card* mk(uint v, Color c) { return new card(static_cast<int>(v), c); }
		
    game_state* gs = nullptr;
    player* p0 = nullptr;
    player* p1 = nullptr;
    player* p2 = nullptr;
};


TEST_F(EventCardEffectTest, DoomsdayEndsGameAndGivesPoints) {
    clear_hands();
    std::string err;
    event_card eve(Event_card_collection::DOOMSDAY);
		
    ASSERT_FALSE(gs->is_finished());
    ASSERT_EQ(0, p0->get_score());
    ASSERT_EQ(0, p1->get_score());
    ASSERT_EQ(0, p2->get_score());
		
    eve.effect(gs, err);
		
    EXPECT_TRUE(gs->is_finished());
    EXPECT_EQ(50, p0->get_score());
    EXPECT_EQ(50, p1->get_score());
    EXPECT_EQ(50, p2->get_score());
}


TEST_F(EventCardEffectTest, EarthquakeRotatesHandsToTheRight) {
    clear_hands();
		
    // p0 has 2 cards, p1 has 1 card, p2 has 3 cards
    card* a0 = mk(1, Color::BLUE);
    card* a1 = mk(2, Color::BLUE);
    card* b0 = mk(3, Color::GREEN);
    card* c0 = mk(4, Color::RED);
    card* c1 = mk(5, Color::RED);
    card* c2 = mk(6, Color::YELLOW);
    p0->add_card(a0);
    p0->add_card(a1);
    p1->add_card(b0);
    p2->add_card(c0);
    p2->add_card(c1);
    p2->add_card(c2);
		
    std::string err;
    event_card eve(Event_card_collection::EARTHQUAKE);
    eve.effect(gs, err);
		
    // Right rotation: i -> (i+1)%n
    const std::vector<card*> p0_expected = {c0, c1, c2};
    const std::vector<card*> p1_expected = {a0, a1};
    const std::vector<card*> p2_expected = {b0};
    EXPECT_EQ(p0_expected, p0->get_hand()->get_cards());
    EXPECT_EQ(p1_expected, p1->get_hand()->get_cards());
    EXPECT_EQ(p2_expected, p2->get_hand()->get_cards());
}


TEST_F(EventCardEffectTest, TimeBombSetsFinalTurnThreeTurnsAhead) {
    std::string err;
    event_card eve(Event_card_collection::TIME_BOMB);
		
    const int before_turn = gs->get_current_turn_idx();
    ASSERT_EQ(-1, gs->get_final_turn());
    eve.effect(gs, err);
    EXPECT_EQ(before_turn + 3, gs->get_final_turn());
}


TEST_F(EventCardEffectTest, ThirdTimeLuckyDrawsThreeCardsForEachPlayer) {
    clear_hands();
		
    const int before_pile = gs->get_draw_pile()->get_nof_cards();
    const int before0 = p0->get_nof_cards();
    const int before1 = p1->get_nof_cards();
    const int before2 = p2->get_nof_cards();
		
    std::string err;
    event_card eve(Event_card_collection::THIRD_TIME_LUCKY);
    eve.effect(gs, err);
		
    EXPECT_EQ(before0 + 3, p0->get_nof_cards());
    EXPECT_EQ(before1 + 3, p1->get_nof_cards());
    EXPECT_EQ(before2 + 3, p2->get_nof_cards());
    EXPECT_EQ(before_pile - 9, gs->get_draw_pile()->get_nof_cards());
}


TEST_F(EventCardEffectTest, CommunismEqualizesHandSizesToMaximum) {
    clear_hands();
		
    	// Make different hand sizes: 1, 4, 2  should all become 4
    p0->add_card(mk(1, Color::BLUE));
		
    p1->add_card(mk(2, Color::GREEN));
    p1->add_card(mk(3, Color::GREEN));
    p1->add_card(mk(4, Color::GREEN));
    p1->add_card(mk(5, Color::GREEN));
		
    p2->add_card(mk(6, Color::RED));
    p2->add_card(mk(7, Color::RED));
		
    const int before_pile = gs->get_draw_pile()->get_nof_cards();
		
    std::string err;
    event_card eve(Event_card_collection::COMMUNISM);
    eve.effect(gs, err);
		
    EXPECT_EQ(4, p0->get_nof_cards());
    EXPECT_EQ(4, p1->get_nof_cards());
    EXPECT_EQ(4, p2->get_nof_cards());
		
    // draws: (4-1) + (4-2) = 5
    EXPECT_EQ(before_pile - 5, gs->get_draw_pile()->get_nof_cards());
}


TEST_F(EventCardEffectTest, RobinHoodSwapsSmallestAndLargestHand) {
    clear_hands();
		
    // sizes: p0=1 smallest, p1=2, p2=4 largest
    card* min0 = mk(1, Color::BLUE);
    p0->add_card(min0);
		
    card* mid0 = mk(2, Color::GREEN);
    card* mid1 = mk(3, Color::GREEN);
    p1->add_card(mid0);
    p1->add_card(mid1);
		
    card* max0 = mk(4, Color::RED);
    card* max1 = mk(5, Color::RED);
    card* max2 = mk(6, Color::RED);
    card* max3 = mk(7, Color::RED);
    p2->add_card(max0);
    p2->add_card(max1);
    p2->add_card(max2);
    p2->add_card(max3);
		
    std::string err;
    event_card eve(Event_card_collection::ROBIN_HOOD);
    eve.effect(gs, err);
		
    const std::vector<card*> p0_expected = {max0, max1, max2, max3};
    const std::vector<card*> p1_expected = {mid0, mid1};
    const std::vector<card*> p2_expected = {min0};
    EXPECT_EQ(p0_expected, p0->get_hand()->get_cards());
    EXPECT_EQ(p1_expected, p1->get_hand()->get_cards());
    EXPECT_EQ(p2_expected, p2->get_hand()->get_cards());
}


TEST_F(EventCardEffectTest, CapitalismMakesEveryoneDrawAsManyCardsAsTheyHave) {
    clear_hands();
		
    // sizes: 1,2,3 , each doubles (draws 1+2+3)
    p0->add_card(mk(1, Color::BLUE));
    p1->add_card(mk(2, Color::GREEN));
    p1->add_card(mk(3, Color::GREEN));
    p2->add_card(mk(4, Color::RED));
    p2->add_card(mk(5, Color::RED));
    p2->add_card(mk(6, Color::RED));
		
    gs->set_current_player_idx(1);
    const int before_pile = gs->get_draw_pile()->get_nof_cards();
		
    std::string err;
    event_card eve(Event_card_collection::CAPITALISM);
    eve.effect(gs, err);
		
    EXPECT_EQ(2, p0->get_nof_cards());
    EXPECT_EQ(4, p1->get_nof_cards());
    EXPECT_EQ(6, p2->get_nof_cards());
    EXPECT_EQ(before_pile - (1 + 2 + 3), gs->get_draw_pile()->get_nof_cards());
}


TEST_F(EventCardEffectTest, MatingSeasonRemovesAllDuplicateNumberSets) {
    clear_hands();
		
    // p0: 5,5,7  remove both 5
    p0->add_card(mk(5, Color::BLUE));
    p0->add_card(mk(5, Color::RED));
    p0->add_card(mk(7, Color::GREEN));
		
    // p1: 2,2,2,8  remove all 2
    p1->add_card(mk(2, Color::BLUE));
    p1->add_card(mk(2, Color::GREEN));
    p1->add_card(mk(2, Color::RED));
    p1->add_card(mk(8, Color::YELLOW));
		
    // p2: all unique unchanged
    card* u0 = mk(1, Color::BLUE);
    card* u1 = mk(3, Color::GREEN);
    card* u2 = mk(9, Color::RED);
    p2->add_card(u0);
    p2->add_card(u1);
    p2->add_card(u2);
		
    std::string err;
    event_card eve(Event_card_collection::MATING_SEASON);
    eve.effect(gs, err);
		
    ASSERT_EQ(1, p0->get_nof_cards());
    EXPECT_EQ(7, p0->get_hand()->get_cards().at(0)->get_value());
		
    ASSERT_EQ(1, p1->get_nof_cards());
    EXPECT_EQ(8, p1->get_hand()->get_cards().at(0)->get_value());
		
    const std::vector<card*> p2_expected = {u0, u1, u2};
    EXPECT_EQ(p2_expected, p2->get_hand()->get_cards());
}


TEST_F(EventCardEffectTest, FridayThe13thDoesNothing) {
    clear_hands();
		
    card* a = mk(1, Color::BLUE);
    card* b = mk(2, Color::GREEN);
    p0->add_card(a);
    p1->add_card(b);
    const int before_pile = gs->get_draw_pile()->get_nof_cards();
		
    std::string err;
    event_card eve(Event_card_collection::FRIDAY_THE_13TH);
    eve.effect(gs, err);
		
    const std::vector<card*> p0_expected = {a};
    const std::vector<card*> p1_expected = {b};
    const std::vector<card*> p2_expected = {};
    EXPECT_EQ(p0_expected, p0->get_hand()->get_cards());
    EXPECT_EQ(p1_expected, p1->get_hand()->get_cards());
    EXPECT_EQ(p2_expected, p2->get_hand()->get_cards());
    EXPECT_EQ(before_pile, gs->get_draw_pile()->get_nof_cards());
    EXPECT_FALSE(gs->is_finished());
}


TEST_F(EventCardEffectTest, FinishLineWrapsUpRoundAndStartsNextRound) {
    clear_hands();
		
    // Put deterministic cards into hands so we can check score
    p0->add_card(mk(1, Color::BLUE));
    p1->add_card(mk(2, Color::GREEN));
    p2->add_card(mk(3, Color::RED));
		
    const int before_round = gs->get_round_number();
    const int p0_before_score = p0->get_score();
    const int p1_before_score = p1->get_score();
    const int p2_before_score = p2->get_score();
    const int p0_hand_score = static_cast<int>(p0->get_hand()->get_score());
    const int p1_hand_score = static_cast<int>(p1->get_hand()->get_score());
    const int p2_hand_score = static_cast<int>(p2->get_hand()->get_score());
		
    std::string err;
    event_card eve(Event_card_collection::FINISH_LINE);
    eve.effect(gs, err);
		
    EXPECT_EQ(before_round + 1, gs->get_round_number());
    EXPECT_EQ(p0_before_score + p0_hand_score, p0->get_score());
    EXPECT_EQ(p1_before_score + p1_hand_score, p1->get_score());
    EXPECT_EQ(p2_before_score + p2_hand_score, p2->get_score());
		
    // setup_round deals 6 cards to each player
    EXPECT_EQ(6, p0->get_nof_cards());
    EXPECT_EQ(6, p1->get_nof_cards());
    EXPECT_EQ(6, p2->get_nof_cards());
}


TEST(EventCardEffectSmokeTest, AllEventCardsEffectDoesNotCrash) {
    for (Event_card_collection typ : all_event_types()) {
        game_state* gs = new game_state();
        player* p0 = new player("A");
        player* p1 = new player("B");
				
        std::string err;
        ASSERT_TRUE(gs->add_player(p0, err)) << err;
        ASSERT_TRUE(gs->add_player(p1, err)) << err;
        ASSERT_TRUE(gs->start_game(err)) << err;
        gs->set_current_player_idx(0);
				
        event_card eve(typ);
        EXPECT_NO_THROW(eve.effect(gs, err)) << "Event card type=" << static_cast<uint>(typ);
				
        delete p0;
        delete p1;
        delete gs;
    }
}
