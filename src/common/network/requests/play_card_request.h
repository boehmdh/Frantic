//
// Created by Manuel on 28.01.2021.
//

#ifndef FRANTIC_PLAY_CARD_REQUEST_H
#define FRANTIC_PLAY_CARD_REQUEST_H

#include "client_request.h"
#include <string>
#include "../../../../rapidjson/include/rapidjson/document.h"

class play_card_request : public client_request {

private:
    std::string _card_id;
    uint _wish;
    uint _ff_target;

    // private constructor for deserialization
    play_card_request(base_class_properties props,
                      std::string card_id,
                      uint wish,
                      uint ff_target);

public:

    // public constructor used by client
    play_card_request(std::string game_id,
                      std::string player_id,
                      std::string card_id,
                      uint wish,
                      uint ff_target);

    uint get_fantastic_four_target() const { return _ff_target; }
    std::string get_card_id() const { return _card_id; }
    uint get_wish() const { return _wish; }

    virtual void write_into_json(rapidjson::Value& json,
                                 rapidjson::Document::AllocatorType& allocator) const override;

    static play_card_request* from_json(const rapidjson::Value& json);
};

#endif


