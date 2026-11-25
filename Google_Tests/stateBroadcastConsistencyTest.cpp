#include "gtest/gtest.h"

#include "../src/common/game_state/game_state.h"
#include "../src/common/game_state/player/player.h"
#include "../src/common/game_state/cards/card.h"

#include "../src/common/network/responses/full_state_response.h"
#include "../src/common/network/responses/request_response.h"
#include "../src/common/serialization/json_utils.h"
#include "../rapidjson/include/rapidjson/document.h"


#include <tuple>
#include <vector>
#include <string>

// helpers to build JSON
static std::string make_broadcast_msg_json(const game_state* gs) {
    // Build exactly what full_state_response would send
    auto* doc = new rapidjson::Document();
    doc->SetObject();
    auto& A = doc->GetAllocator();

    doc->AddMember("type", rapidjson::Value("full_state_msg", A), A);
    doc->AddMember("game_id", rapidjson::Value(gs->get_id().c_str(), A), A);

    rapidjson::Document* state = gs->to_json();
    rapidjson::Value state_copy(rapidjson::kObjectType);
    state_copy.CopyFrom(*state, A);   // copy into doc allocator
    delete state;

    doc->AddMember("state_json", state_copy, A);

    std::string s = json_utils::to_string(doc);
    delete doc;
    return s;
}

static std::string make_request_response_json(const game_state* gs,
                                              const std::string& req_id = "ST01",
                                              bool success = true,
                                              const std::string& err = "") {
    // Build exactly what request_response would send
    auto* doc = new rapidjson::Document();
    doc->SetObject();
    auto& A = doc->GetAllocator();

    doc->AddMember("type", rapidjson::Value("req_response", A), A);
    doc->AddMember("game_id", rapidjson::Value(gs->get_id().c_str(), A), A);

    doc->AddMember("err", rapidjson::Value(err.c_str(), A), A);
    doc->AddMember("req_id", rapidjson::Value(req_id.c_str(), A), A);
    doc->AddMember("success", success, A);

    rapidjson::Document* state = gs->to_json();
    rapidjson::Value state_copy(rapidjson::kObjectType);
    state_copy.CopyFrom(*state, A);   // copy into doc allocator
    delete state;

    doc->AddMember("state_json", state_copy, A);

    std::string s = json_utils::to_string(doc);
    delete doc;
    return s;
}


// Unwrap state_json from response messages
static const rapidjson::Value& unwrap_state_json(const rapidjson::Document& d) {
    if (d.HasMember("state_json")) return d["state_json"];
    return d;
}

//JSON field helpers

static bool read_uint_like(const rapidjson::Value& v, uint& out) {
    if (v.IsUint()) { out = v.GetUint(); return true; }
    if (v.IsInt() && v.GetInt() >= 0) { out = static_cast<uint>(v.GetInt()); return true; }


    if (v.IsObject() && v.HasMember("value")) {
        const auto& inner = v["value"];
        if (inner.IsUint()) { out = inner.GetUint(); return true; }
        if (inner.IsInt() && inner.GetInt() >= 0) { out = static_cast<uint>(inner.GetInt()); return true; }
    }
    return false;
}

static bool get_uint_field(const rapidjson::Value& obj, const char* key, uint& out) {
    if (!obj.IsObject() || !obj.HasMember(key)) return false;
    return read_uint_like(obj[key], out);
}

//signature extraction

struct StateSig {
    std::tuple<uint, uint, uint> top;   // (value, color, action)
    uint wish = 0;
    std::string current;                // idx/name/id
    std::vector<uint> hand_sizes;        // per player, only sizes
};

static bool extract_card_sig(const rapidjson::Value& v, std::tuple<uint, uint, uint>& out) {
    if (!v.IsObject()) return false;

    uint value = 0, color = 0, action = 0;
    if (!get_uint_field(v, "value", value)) return false;
    if (!get_uint_field(v, "color", color)) return false;

    // action is optional
    (void)get_uint_field(v, "action", action);

    out = { value, color, action };
    return true;
}

static bool extract_top_card_from_state(const rapidjson::Value& st, std::tuple<uint,uint,uint>& out) {
    if (st.IsObject() && st.HasMember("discard_pile") && st["discard_pile"].IsObject()) {
        const auto& dp = st["discard_pile"];
        if (dp.HasMember("top_card")) return extract_card_sig(dp["top_card"], out);
        if (dp.HasMember("cards") && dp["cards"].IsArray() && dp["cards"].Size() > 0) {
            return extract_card_sig(dp["cards"][dp["cards"].Size() - 1], out);
        }
    }
    if (st.IsObject() && st.HasMember("top_card")) return extract_card_sig(st["top_card"], out);
    return false;
}

static uint extract_wish_from_state(const rapidjson::Value& st) {
    if (!st.IsObject() || !st.HasMember("wish")) return 0;

    uint w = 0;
    if (read_uint_like(st["wish"], w)) return w;

    // Sometimes wish is absent.
    return 0;
}

static std::string extract_current_player_from_state(const rapidjson::Value& st) {
    if (!st.IsObject()) return "";

    if (st.HasMember("current_player")) {
        const auto& cp = st["current_player"];
        if (cp.IsUint()) return std::to_string(cp.GetUint());
        if (cp.IsInt())  return std::to_string(cp.GetInt());
        if (cp.IsString()) return cp.GetString();

        if (cp.IsObject()) {
            if (cp.HasMember("id") && cp["id"].IsString()) return cp["id"].GetString();
            if (cp.HasMember("name") && cp["name"].IsString()) return cp["name"].GetString();

            if (cp.HasMember("value")) {
                uint tmp = 0;
                if (read_uint_like(cp, tmp)) return std::to_string(tmp);
            }
        }
    }

    if (st.HasMember("current_player_idx")) {
        uint idx = 0;
        if (read_uint_like(st["current_player_idx"], idx)) return std::to_string(idx);
    }

    return "";
}

static uint extract_hand_size_from_player_obj(const rapidjson::Value& p) {

    if (!p.IsObject()) return 0;

    if (p.HasMember("hand")) {
        const auto& h = p["hand"];
        if (h.IsObject() && h.HasMember("cards") && h["cards"].IsArray()) return h["cards"].Size();
        if (h.IsArray()) return h.Size();
    }
    if (p.HasMember("cards") && p["cards"].IsArray()) return p["cards"].Size();

    // Some broadcast only a hand_size
    if (p.HasMember("hand_size")) {
        uint hs = 0;
        if (read_uint_like(p["hand_size"], hs)) return hs;
    }

    return 0;
}

static StateSig signature_from_response_json(const std::string& response_json) {
    rapidjson::Document d;
    d.Parse(response_json.c_str());

    const rapidjson::Value& st = unwrap_state_json(d);

    StateSig s;
    s.wish = extract_wish_from_state(st);
    s.current = extract_current_player_from_state(st);

    bool ok_top = extract_top_card_from_state(st, s.top);
    EXPECT_TRUE(ok_top) << "Could not extract top card from state_json.";

    if (st.IsObject() && st.HasMember("players") && st["players"].IsArray()) {
        for (const auto& p : st["players"].GetArray()) {
            s.hand_sizes.push_back(extract_hand_size_from_player_obj(p));
        }
    } else {
        ADD_FAILURE() << "No 'players' array found in state_json.";
    }

    return s;
}

// ST01 test

TEST(SystemTest, ST01_GameStateBroadcastConsistency) {
#ifdef FRANTIC_SERVER
    std::string err;

    auto* gs = new game_state();
    auto* c1 = new player("Client1");
    auto* c2 = new player("Client2");
    auto* c3 = new player("Client3");

    ASSERT_TRUE(gs->add_player(c1, err)) << err;
    ASSERT_TRUE(gs->add_player(c2, err)) << err;
    ASSERT_TRUE(gs->add_player(c3, err)) << err;

    // Make Client1 the current player.
    gs->set_current_player_idx(0);

    // Action: Client1 plays a valid card.
    // a Rainbow card is always playable, plus it sets a wish.
    card* rainbow = new card(20, Color::RAINBOW);
    c1->add_card(rainbow);

    uint ff_target = 0;
    uint wish = static_cast<uint>(Color::RED);
    ASSERT_TRUE(gs->play_card(c1, rainbow->get_id(), wish, ff_target, err)) << err;

    // Client1 gets request_response JSON, others get full_state_response JSON.
    const std::string j_sender = make_request_response_json(gs, "ST01");
    const std::string j_c2     = make_broadcast_msg_json(gs);
    const std::string j_c3     = make_broadcast_msg_json(gs);

    const StateSig s1 = signature_from_response_json(j_sender);
    const StateSig s2 = signature_from_response_json(j_c2);
    const StateSig s3 = signature_from_response_json(j_c3);

    // expected, all identical for publicly comparable parts.
    EXPECT_EQ(s1.wish, s2.wish);
    EXPECT_EQ(s1.wish, s3.wish);

    EXPECT_EQ(s1.current, s2.current);
    EXPECT_EQ(s1.current, s3.current);

    EXPECT_EQ(s1.top, s2.top);
    EXPECT_EQ(s1.top, s3.top);

    EXPECT_EQ(s1.hand_sizes, s2.hand_sizes);
    EXPECT_EQ(s1.hand_sizes, s3.hand_sizes);

    delete gs;
    delete c1; delete c2; delete c3;
#endif
}

