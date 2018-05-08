#include <iostream>
#include <list>

#include "context.h"

Context::Context()
{
#ifdef DEBUG_PRINT
    std::cout << "Context::Context, this==" << this << std::endl;
#endif
}

Context::~Context()
{
#ifdef DEBUG_PRINT
    std::cout << "Context::~Context, this==" << this << std::endl;
#endif
    Stop();
}

void Context::SetProcessor(std::shared_ptr<CommandProcessor> aCommandProcessor)
{
    mCommandProcessor = aCommandProcessor;
}

void Context::Start()
{
#ifdef DEBUG_PRINT
    std::cout << "Context::Start, this==" << this << std::endl;
#endif

    mDone = false;
    mNotified = false;
    mStream.clear();
    mStream.str("");

    mThread = std::move(std::thread(&Context::ThreadProc, this, mCommandProcessor));
}

void Context::ProcessData(const char* aData, std::size_t aSize)
{
#ifdef DEBUG_PRINT
    std::cout << "Context::ProcessData, this==" << this << ", aData=" << aData << ", aSize=" << aSize << ", mDone=" << mDone.load() << std::endl;
#endif
    if (mDone.load())
        return;
    {
        std::lock_guard<std::mutex> lk(mStreamMutex);
#ifdef DEBUG_PRINT
        std::cout << "Context::ProcessData 2, pos = " << mStream.tellp() << std::endl;
#endif
        mStream.write(aData, aSize);
#ifdef DEBUG_PRINT
        std::cout << "Context::ProcessData 3, pos = " << mStream.tellp() << std::endl;
#endif
    }
    mNotified = true;
    mCondition.notify_one();
#ifdef DEBUG_PRINT
    std::cout << "Context::ProcessData end, this==" << this << ", stream=" << mStream.str() << std::endl;
#endif
}

void Context::Stop()
{
#ifdef DEBUG_PRINT
    std::cout << "Context::Stop1, this==" << this << ", stream=" << mStream.str() << std::endl;
#endif
    mDone = true;
    mNotified = true;
#ifdef DEBUG_PRINT
    std::cout << "Context::Stop2, this==" << this << std::endl;
#endif
    mCondition.notify_one();
#ifdef DEBUG_PRINT
    std::cout << "Context::Stop3, this==" << this << std::endl;
#endif
    if (mThread.joinable())
        mThread.join();
#ifdef DEBUG_PRINT
    std::cout << "Context::Stop4, this==" << this << std::endl;
#endif
}

void Context::ProcessStream(std::shared_ptr<CommandProcessor> aCommandProcessor)
{
    std::list<std::string> text;
    {
        std::string line;
        std::lock_guard<std::mutex> lk(mStreamMutex);
        mStream.seekp(0);
#ifdef DEBUG_PRINT
        std::cout << "Context::ProcessStream 1, pos = " << mStream.tellp() << ", str=" <<  mStream.str() << std::endl;
#endif
        while (!std::getline(mStream, line).eof())
        {
            if (line.length() > 0 && line[line.length() - 1] == '\r')
            {
                 line = line.substr(0, line.length() - 1);
            }
#ifdef DEBUG_PRINT
            std::cout << "Context::ProcessStream 2, line = " << line << ";" << std::endl;
#endif
            text.push_back(line);
        }
#ifdef DEBUG_PRINT
        std::cout << "Context::ProcessStream 3, pos = " << mStream.tellp() << std::endl;
#endif
        mStream.clear();
        mStream.str("");
#ifdef DEBUG_PRINT
        std::cout << "Context::ProcessStream 4, stream = " << mStream.str() << ", pos = " << mStream.tellp() << std::endl;
#endif
        mStream.write(line.c_str(), line.size());
#ifdef DEBUG_PRINT
        std::cout << "Context::ProcessStream 5, pos = " << mStream.tellp() << ", line = " << line << std::endl;
#endif
    }
    for (const auto& line: text)
    {
#ifdef DEBUG_PRINT
        std::cout << "Context::ProcessStream 6, line=" << line << ";" << std::endl;
#endif
        aCommandProcessor->ProcessLine(line);
#ifdef DEBUG_PRINT
        std::cout << "Context::ProcessStream 7, line=" << line << std::endl;
#endif
    }
}

void Context::ThreadProc(std::shared_ptr<CommandProcessor> aCommandProcessor)
{
#ifdef DEBUG_PRINT
    std::cout << "Context::ThreadProc start, this==" << this << std::endl;
#endif
    try
    {
        while (!mDone.load())
        {
#ifdef DEBUG_PRINT
            std::cout << "Context::ThreadProc0, this==" << this << std::endl;
#endif
            std::unique_lock<std::mutex> lk(mStreamMutex);
            while (!mNotified.load())
                mCondition.wait(lk);
#ifdef DEBUG_PRINT
            std::cout << "Context::ThreadProc01, this==" << this << std::endl;
#endif
            lk.unlock();
            ProcessStream(aCommandProcessor);
            mNotified = false;
        }
#ifdef DEBUG_PRINT
        std::cout << "Context::ThreadProc1, this==" << this << std::endl;
#endif
        ProcessStream(aCommandProcessor);
        aCommandProcessor->DumpCounters();
#ifdef DEBUG_PRINT
        std::cout << "Context::ThreadProc2, this==" << this << std::endl;
#endif
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
    }
#ifdef DEBUG_PRINT
    std::cout << "Context::ThreadProc end, this==" << this << std::endl;
#endif
}
