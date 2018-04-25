#include <iostream>

#include "session.h"
#include "inputprocessor.h"

Session::Session(tcp::socket aSocket, std::shared_ptr<CommandProcessor> aCommandProcessor)
    : mSocket(std::move(aSocket))
{
    auto processor = std::make_shared<InputProcessor>("main", std::vector<std::shared_ptr<ICommandProcessor>>{aCommandProcessor});
    mContext.SetProcessor(processor);

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
    mContext.Start();
    DoRead();
}

void Session::Stop()
{
    mContext.Stop();
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
            else
                Stop();
        });
}

void Session::Deliver(std::size_t length)
{
#ifdef DEBUG_PRINT
    std::cout << "Session::Deliver, this==" << this << ", mReadMsg.data()=" << mReadMsg.data() << ", mReadMsg.size()=" << mReadMsg.size() << std::endl;
#endif

    mContext.ProcessData(mReadMsg.data(), length);
}
