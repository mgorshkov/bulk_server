#pragma once

#include <vector>
#include <memory>
#include <string>

#include "structs.h"
#include "icommandprocessor.h"

using CommandProcessors = std::vector<std::shared_ptr<ICommandProcessor>>;

class CommandProcessor : public ICommandProcessor
{
public:
    CommandProcessor(const std::string& aName, const CommandProcessors& aDependentCommandProcessors = CommandProcessors());

    void StartBlock() override;
    void FinishBlock() override;

    void ProcessLine(const std::string& aLine) override;

    void ProcessCommand(const Command& aCommand) override;

    void ProcessBatch(const CommandBatch& aCommandBatch) override;

    void Start() override;
    void Stop() override;

protected:
    std::string mName;

private:
    CommandProcessors mDependentCommandProcessors;
};
