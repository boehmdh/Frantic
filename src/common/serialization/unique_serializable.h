//
//
//
// Used to serialize game_state objects that need to be identifiable by a unique id.

#ifndef FRANTIC_UNIQUE_SERIALIZABLE_H
#define FRANTIC_UNIQUE_SERIALIZABLE_H


#include "serializable.h"

class unique_serializable : public serializable {
protected:

    std::string _id;       // unique identifier

    unique_serializable();
    unique_serializable(std::string id);

public:
// accessors
    std::string get_id() const;

// serializable interface
    virtual void write_into_json(rapidjson::Value& json,
                                 rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator> &allocator) const override;

};

#endif //FRANTIC_UNIQUE_SERIALIZABLE_H
