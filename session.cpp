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

    boost::asio::async_read_until(mSocket, mBuffer, '\n',
        [this, self](boost::system::error_code ec, std::size_t /*length*/)
        {
#ifdef DEBUG_PRINT
            std::cout << "Session::DoRead, this==" << this << ", ec=" << ec << std::endl;
#endif
            Deliver();

            if (!ec)
                DoRead();
            else
                Stop();
        });
}

void Session::Deliver()
{
#ifdef DEBUG_PRINT
    std::cout << "Session::Deliver, this==" << this  << std::endl;
#endif

    mContext.ProcessData(&mBuffer);
}
