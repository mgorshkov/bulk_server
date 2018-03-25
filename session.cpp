#include "session.h"

#include "threadedcommandprocessor.h"
#include "threadedcommandprocessorimpl.h"
#include "consoleoutput.h"
#include "inputprocessor.h"
#include "batchcommandprocessor.h"
#include "reportwriter.h"

#define THREADS 2

std::shared_ptr<CommandProcessor> CreateInputContext(std::size_t bulkSize)
{
    auto reportWritersThreadedProcessor = std::make_shared<ThreadedCommandProcessor<ReportWriter>>("file", THREADS);
    auto consoleOutputThreadedProcessor = std::make_shared<ThreadedCommandProcessor<ConsoleOutput>>("log");

    CommandProcessors processors = {reportWritersThreadedProcessor, consoleOutputThreadedProcessor};
    auto batchCommandProcessor = std::make_shared<BatchCommandProcessor>("main", bulkSize, processors);
    CommandProcessors batchCommandProcessors = {batchCommandProcessor};
    auto inputCommandProcessor = std::make_shared<InputProcessor>("main", batchCommandProcessors);
    
    reportWritersThreadedProcessor->SetContext(inputCommandProcessor.get());
    consoleOutputThreadedProcessor->SetContext(inputCommandProcessor.get());
    batchCommandProcessor->SetContext(inputCommandProcessor.get());

    reportWritersThreadedProcessor->Start();
    consoleOutputThreadedProcessor->Start();

    return inputCommandProcessor;
}

Session::Session(int aBulkSize, tcp_socket aSocket)
    : mSocket(std::move(aSocket))
    : mContext(CreateInputContext(aBulkSize))
{
}

void Session::Start()
{
    DoRead();
}

void Session::DoRead()
{
    auto self(shared_from_this());

    boost::asio::async_read(mSocket,
        boost::asio::buffer(mReadMsg.data(), mReadMsg.size()),
        [this, self](boost::system::error_code ec, std::size_t /*length*/)
        {
            if (!ec)
              Deliver(mReadMsg);
        });
}
  
void Session::Deliver(const std::string& aMessage)
{
    mContext.ProcessData(aMessage.c_str(), aMessage.length());
}
