#include <iostream>

#include <boost/asio.hpp>

using boost::asio::ip::tcp;

class BulkClient
{
public:
    BulkClient(boost::asio::io_service& aIoService, tcp::resolver::iterator aEndpointIterator)
        : mIoService(aIoService)
        , mSocket(aIoService)
    {
        DoConnect(aEndpointIterator);
    }

    void write(const std::string& aMsg)
    {
        mIoService.post(
            [this, aMsg]()
            {
                bool writeInProgress = !mWriteMsgs.empty();
                mWriteMsgs.push_back(aMsg);
                if (!writeInProgress)
                {
                    doWrite();
                }
            });
    }

    void close()
    {
        mIoService.post([this]() { mSocket.close(); });
    }

private:
    void DoConnect(tcp::resolver::iterator aEndpointIterator)
    {
        boost::asio::async_connect(mSocket, aEndpointIterator);
    }

    void DoWrite()
    {
        boost::asio::async_write(mSocket,
            boost::asio::buffer(mWriteMsgs.front().c_str(),
                mWriteMsgs.front().size()),
        [this](boost::system::error_code ec, std::size_t /*length*/)
        {
            if (!ec)
            {
                mWriteMsgs.pop_front();
                if (!mWriteMsgs.empty())
                    DoWrite();
            }
            else
                mSocket.close();
        });
    }

private:
    boost::asio::io_service& mIoService;
    tcp::socket mSocket;
    BulkQueue mWriteMsgs;
};

int main(int, char *[])
{
    try
    {
        if (argc != 3)
        {
            std::cerr << "Usage: bulk_client <host> <port>\n";
            return 1;
        }

        boost::asio::io_service ioService;

        tcp::resolver resolver(ioService);
        auto endpointIterator = resolver.resolve({ argv[1], argv[2] });
        BulkClient client(ioService, endpointIterator);

        std::thread t([&ioService]()
            {
                ioService.run();
            });

        client.write("a");
        client.write("b");
        client.write("\n");
        client.write("c");
        client.write("\n");
        client.write("d");
        client.write("\n");
        client.write("\n");
        client.write("\n");
        client.write("f");
        client.write("\n");
        client.write("g");
        client.write("{");
        client.write("\n");
        client.write("h");
        client.write("\n");
        client.write("i");
        client.write("\n");
        client.write("j");
        client.write("\n");
        client.write("k");
        client.write("\n");
        client.write("\n");
        client.write("l");
        client.write("\n");

        client.close();
        t.join();
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}
