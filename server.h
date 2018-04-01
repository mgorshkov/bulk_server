#pragma once

#include "includes.h"
#include "commandprocessor.h"

class Server
{
public:
    Server(boost::asio::io_service& aIoService, const tcp::endpoint& aEndpoint, std::size_t aBulkSize);

private:
    void DoAccept();

    tcp::acceptor mAcceptor;
    tcp::socket mSocket;
    std::size_t mBulkSize;
    std::shared_ptr<CommandProcessor> mCommandProcessor;
};
