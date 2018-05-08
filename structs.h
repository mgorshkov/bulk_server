#pragma once

#include <string>
#include <vector>
#include <chrono>
#include <deque>

using Timestamp = std::chrono::system_clock::time_point;

struct Command
{
    std::string mText;
    Timestamp mTimestamp;
};

struct CommandBatch
{
    std::vector<Command> mCommands;
    Timestamp mTimestamp;

    std::size_t Size() const
    {
        return mCommands.size();
    }
};

using BulkQueue = std::deque<std::string>;