#pragma once

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

    void ProcessData(const char* aData, std::size_t aSize);

    void Start();
    void Stop();

private:
    void ProcessStream(std::shared_ptr<CommandProcessor> aCommandProcessor);

    static void ThreadProc(Context* aContext, std::shared_ptr<CommandProcessor> aCommandProcessor);

    std::shared_ptr<CommandProcessor> mCommandProcessor;

    std::stringstream mStream;
    std::mutex mStreamMutex;
    std::condition_variable mCondition;
    std::atomic_bool mDone{false};
    std::atomic_bool mNotified{false};

    std::thread mThread;
};
