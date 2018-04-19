#pragma once

#include <boost/asio.hpp>

#include "includes.h"
#include "context.h"

class Session : public std::enable_shared_from_this<Session>
{
public:
    Session(tcp::socket aSocket, std::shared_ptr<CommandProcessor> aCommandProcessor);
    ~Session();

    void Start();

private:
    void Stop();

    void DoRead();
    void Deliver();

    Context mContext;
    tcp::socket mSocket;
    boost::asio::streambuf mBuffer;
};
