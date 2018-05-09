#include <iostream>
#include <list>

#include "session.h"
#include "inputprocessor.h"

Session::Session(tcp::socket aSocket, std::shared_ptr<CommandProcessor> aCommandProcessor)
    : mSocket(std::move(aSocket))
    , mProcessor("main", std::vector<std::shared_ptr<ICommandProcessor>>{aCommandProcessor})
{
#ifdef DEBUG_PRINT
    std::cout << "Session::Session, this==" << this << std::endl;
#endif
}

Session::~Session()
{
#ifdef DEBUG_PRINT
    std::cout << "Session::~Session, this==" << this << std::endl;
#endif
}

void Session::Start()
{
    DoRead();
}

void Session::DoRead()
{
    auto self(shared_from_this());

    boost::asio::async_read(mSocket, boost::asio::buffer(mReadMsg),
        [this, self](boost::system::error_code ec, std::size_t length)
        {
#ifdef DEBUG_PRINT
            std::cout << "Session::DoRead, this==" << this << ", ec=" << ec << ", mReadMsg=" << &mReadMsg[0] << ", length=" << length << std::endl;
#endif
            Deliver(length);

            if (!ec)
                DoRead();
        });
}

void Session::Deliver(std::size_t length)
{
#ifdef DEBUG_PRINT
    std::cout << "Session::Deliver, this==" << this << ", mReadMsg.data()=" << mReadMsg.data() << ", mReadMsg.size()=" << mReadMsg.size() << std::endl;
#endif

    mStream.write(mReadMsg.data(), length);

    ProcessStream();
}

void Session::ProcessStream()
{
    std::list<std::string> text;
    {
        std::string line;
        mStream.seekp(0);
        while (!std::getline(mStream, line).eof())
        {
            if (line.length() > 0 && line[line.length() - 1] == '\r')
            {
                 line = line.substr(0, line.length() - 1);
            }
            text.push_back(line);
        }
        mStream.clear();
        mStream.str("");
        mStream.write(line.c_str(), line.size());
    }
    for (const auto& line: text)
    {
        mProcessor.ProcessLine(line);
    }
}
