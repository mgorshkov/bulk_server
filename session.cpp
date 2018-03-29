#include "session.h"

#include "threadedcommandprocessor.h"
#include "threadedcommandprocessorimpl.h"
#include "consoleoutput.h"
#include "inputprocessor.h"
#include "batchcommandprocessor.h"
#include "reportwriter.h"

#define THREADS 2

static std::shared_ptr<CommandProcessor> CreateInputContext(std::size_t bulkSize)
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

Session::Session(tcp::socket aSocket, std::size_t aBulkSize)
    : mSocket(std::move(aSocket))
    , mContext(CreateInputContext(aBulkSize))
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
        boost::asio::buffer(mReadMsg),
        [this, self](boost::system::error_code ec, std::size_t /*length*/)
        {
            if (!ec)
            {
                Deliver();
                DoRead();
            }
        });
}
  
void Session::Deliver()
{
#ifdef DEBUG_PRINT
    std::cout << "Session::Deliver, this==" << this << ", mReadMsg.data()=" << mReadMsg.data() << ", mReadMsg.size()=" << mReadMsg.size() << std::endl;
#endif

    mContext.ProcessData(mReadMsg.data(), mReadMsg.size());
}
