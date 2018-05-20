#include "batchcommandprocessor.h"
#include <iostream>

BatchCommandProcessor::BatchCommandProcessor(const std::string& aName, std::size_t aBulkSize, const CommandProcessors& aDependentCommandProcessors)
    : CommandProcessor(aName, aDependentCommandProcessors)
    , mBulkSize(aBulkSize)
    , mBlockForced(false)
    , mAutoDumpThread(&BatchCommandProcessor::ThreadProc, this)
    , mDone(false)
{
}

void BatchCommandProcessor::StartBlock()
{
    mBlockForced = true;
    DumpBatch();
}

void BatchCommandProcessor::FinishBlock()
{
    mBlockForced = false;
    DumpBatch();
}

void BatchCommandProcessor::ProcessCommand(const Command& aCommand)
{
    mCommandBatch.push_back(aCommand);

    if (!mBlockForced && mCommandBatch.size() >= mBulkSize)
    {
        DumpBatch();
    }

    mLastCommandTimestamp = std::chrono::system_clock::now();
}

void BatchCommandProcessor::Stop()
{
    if (!mBlockForced)
        DumpBatch();
    CommandProcessor::Stop();
    mDone = true;
}

void BatchCommandProcessor::ClearBatch()
{
    mCommandBatch.clear();
}

void BatchCommandProcessor::DumpBatch()
{
    std::unique_lock<std::mutex> lk(mBatchMutex);

    if (mCommandBatch.empty())
        return;
    CommandBatch commandBatch{mCommandBatch, mCommandBatch[0].mTimestamp};
    CommandProcessor::ProcessBatch(commandBatch);
    ClearBatch();
}

void BatchCommandProcessor::ThreadProc()
{
    try
    {
        while (!mDone.load())
        {
            auto timestamp = std::chrono::system_clock::now();

            if (mLastCommandTimestamp && timestamp - *mLastCommandTimestamp > std::chrono::milliseconds{400})
		        DumpBatch();
		    
		    std::this_thread::sleep_for(std::chrono::milliseconds{100});
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }
}
