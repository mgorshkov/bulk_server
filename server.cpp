#include <map>
#include <sstream>
#include <iostream>

#include "server.h"
#include "session.h"

Server::Server(boost::asio::io_service& aIoService, const tcp::endpoint& aEndPoint, int aBulkSize)
    : mAcceptor(aIoService, aEndPoint)
    , mSocket(aIoService)
    , mBulkSize(aBulkSize)
{
    DoAccept();
}

void Server::DoAccept()
{
    mAcceptor.async_accept(mSocket,
        [this](boost::system::error_code ec)
        {
            if (!ec)
                std::make_shared<Session>(std::move(mSocket), mBulkSize)->Start();

            DoAccept();
        });
}
