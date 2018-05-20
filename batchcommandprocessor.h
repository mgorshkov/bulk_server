#pragma once

#include <vector>
#include <thread>
#include <mutex>
#include <atomic>
#include <chrono>
#include <optional>

#include "structs.h"
#include "commandprocessor.h"

class BatchCommandProcessor : public CommandProcessor
{
public:
    BatchCommandProcessor(const std::string& aName, std::size_t aBulkSize, const CommandProcessors& aDependentCommandProcessors);

    void StartBlock() override;
    void FinishBlock() override;

    void ProcessCommand(const Command& aCommand) override;
    void Stop() override;

private:
    void ClearBatch();
    void DumpBatch();

    void ThreadProc();

    std::size_t mBulkSize;
    bool mBlockForced;
    std::vector<Command> mCommandBatch;
    std::thread mAutoDumpThread;
    std::optional<std::chrono::system_clock::time_point> mLastCommandTimestamp;
    std::mutex mBatchMutex;
    std::atomic<bool> mDone;
};
