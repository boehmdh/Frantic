//
// Created by Manuel on 09.03.2021.
//

#ifndef FRANTIC_FRANTICEXCEPTION_H
#define FRANTIC_FRANTICEXCEPTION_H

#include <string>

class FranticException : public std::exception {
private:
    std::string _msg;
public:
    explicit FranticException(const std::string& message) : _msg(message) { };

    const char* what() const noexcept override {
        return _msg.c_str();
    }
};

#endif //FRANTIC_FRANTICEXCEPTION_H
