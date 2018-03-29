#pragma once

#include <array>

#include "includes.h"
#include "context.h"

class Session : public std::enable_shared_from_this<Session>
{
public:
    Session(tcp::socket aSocket, std::size_t aBulkSize);
    
    void Start();

private:
    void DoRead();
    void Deliver();

    Context mContext;
    tcp::socket mSocket;
    std::array<char, 1> mReadMsg;
};
