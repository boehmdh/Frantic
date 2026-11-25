//
// Created by Manuel on 28.01.2021.
//

#include "play_card_request.h"

#include "../../serialization/unique_serializable.h"


// PRIVATE CONSTRUCTOR (SERVER SIDE)
play_card_request::play_card_request(
        base_class_properties props,
        std::string card_id,
        uint wish,
        uint ff_target)
    : client_request(props),
      _card_id(card_id),
      _wish(wish),
      _ff_target(ff_target)
{ }

// PUBLIC CONSTRUCTOR (CLIENT SIDE)
play_card_request::play_card_request(
        std::string game_id,
        std::string player_id,
        std::string card_id,
        uint wish,
        uint ff_target)
    : client_request(
          client_request::create_base_class_properties(
                RequestType::play_card,
                uuid_generator::generate_uuid_v4(),
                player_id,
                game_id
          )
      ),
      _card_id(card_id),
      _wish(wish),
      _ff_target(ff_target)
{ }

// SERIALIZATION
void play_card_request::write_into_json(rapidjson::Value& json,
                                        rapidjson::Document::AllocatorType& allocator) const
{
    client_request::write_into_json(json, allocator);

    json.AddMember("card_id", rapidjson::Value(_card_id.c_str(), allocator), allocator);
    json.AddMember("wish", rapidjson::Value(_wish), allocator);
    json.AddMember("ff_target", rapidjson::Value(_ff_target), allocator);
}

// DESERIALIZATION
play_card_request* play_card_request::from_json(const rapidjson::Value& json)
{
    base_class_properties props = client_request::extract_base_class_properties(json);

    return new play_card_request(
        props,
        json["card_id"].GetString(),
        json["wish"].GetUint(),
        json["ff_target"].GetUint()
    );
}


