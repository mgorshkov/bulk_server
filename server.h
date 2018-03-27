#pragma once

#include "includes.h"

class Server
{
public:
    Server(boost::asio::io_service& aIoService, const tcp::endpoint& aEndpoint, std::size_t aBulkSize);

private:
    void DoAccept();

    tcp::acceptor mAcceptor;
    tcp::socket mSocket;
    std::size_t mBulkSize;
};
