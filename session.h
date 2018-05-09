#pragma once

#include <array>

#include "includes.h"
#include "inputprocessor.h"

class Session : public std::enable_shared_from_this<Session>
{
public:
    Session(tcp::socket aSocket, std::shared_ptr<CommandProcessor> aCommandProcessor);
    ~Session();

    void Start();

private:
    void DoRead();
    void Deliver(std::size_t length);
    void ProcessStream();

    tcp::socket mSocket;
    InputProcessor mProcessor;
    static const std::size_t BufSize = 1;
    std::array<char, BufSize> mReadMsg;
    std::stringstream mStream;
};
