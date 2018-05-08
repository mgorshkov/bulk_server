#include <iostream>

#include "commandprocessor.h"

CommandProcessor::CommandProcessor(const std::string& aName, const CommandProcessors& aDependentCommandProcessors)
    : mName(aName)
    , mDependentCommandProcessors(aDependentCommandProcessors)
{
}

void CommandProcessor::StartBlock()
{
    for (auto dependentCommandProcessor : mDependentCommandProcessors)
        dependentCommandProcessor->StartBlock();
}

void CommandProcessor::FinishBlock()
{
    for (auto dependentCommandProcessor : mDependentCommandProcessors)
        dependentCommandProcessor->FinishBlock();
}

void CommandProcessor::ProcessLine(const std::string& aLine)
{
    for (auto dependentCommandProcessor : mDependentCommandProcessors)
        dependentCommandProcessor->ProcessLine(aLine);
}

void CommandProcessor::ProcessCommand(const Command& aCommand)
{
    for (auto dependentCommandProcessor : mDependentCommandProcessors)
        dependentCommandProcessor->ProcessCommand(aCommand);
}

void CommandProcessor::ProcessBatch(const CommandBatch& aCommandBatch)
{
    for (auto dependentCommandProcessor : mDependentCommandProcessors)
    {
        dependentCommandProcessor->ProcessBatch(aCommandBatch);
    }
}

void CommandProcessor::Start()
{
    for (auto dependentCommandProcessor : mDependentCommandProcessors)
        dependentCommandProcessor->Start();
}

void CommandProcessor::Stop()
{
#ifdef DEBUG_PRINT
    std::cout << "CommandProcessor::Stop1()" << this << std::endl;
#endif
    Stop();
#ifdef DEBUG_PRINT
    std::cout << "CommandProcessor::Stop2()" << this << std::endl;
#endif
    for (auto dependentCommandProcessor : mDependentCommandProcessors)
        dependentCommandProcessor->Stop();
#ifdef DEBUG_PRINT
    std::cout << "CommandProcessor::Stop3()" << this << std::endl;
#endif
}

