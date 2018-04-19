#pragma once

#include <boost/asio.hpp>
#include <memory>
#include <mutex>
#include <thread>
#include <sstream>
#include <condition_variable>
#include <atomic>

#include "commandprocessor.h"

class Context
{
public:
    Context();
    ~Context();

    void SetProcessor(std::shared_ptr<CommandProcessor> aCommandProcessor);

    void ProcessData(boost::asio::streambuf* aStream);

    void Start();
    void Stop();

private:
    void ProcessStream(std::shared_ptr<CommandProcessor> aCommandProcessor);

    void ThreadProc(std::shared_ptr<CommandProcessor> aCommandProcessor);

    std::shared_ptr<CommandProcessor> mCommandProcessor;

    std::stringstream mStream;
    std::mutex mStreamMutex;
    std::condition_variable mCondition;
    std::atomic_bool mDone{false};
    std::atomic_bool mNotified{false};

    std::thread mThread;
};
