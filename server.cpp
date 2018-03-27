#include <map>
#include <sstream>
#include <iostream>

#include "server.h"
#include "session.h"

Server::Server(boost::asio::io_service& aIoService, const tcp::endpoint& aEndPoint, std::size_t aBulkSize)
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

int main(int argc, char* argv[])
{
    try
    {
        if (argc < 2)
        {
           std::cerr << "Usage: bulk_server <port> <bulk_size>" << std::endl;
           return 1;
        }

        boost::asio::io_service ioService;

        int port = std::atoi(argv[1]);
        std::size_t bulkSize = std::atoi(argv[2]);
        tcp::endpoint endpoint(tcp::v4(), port);
        Server server(ioService, endpoint, bulkSize);

        ioService.run();
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}
