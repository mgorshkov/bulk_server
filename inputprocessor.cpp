#include <assert.h>
#include <iostream>

#include "inputprocessor.h"

InputProcessor::InputProcessor(const std::string& aName, const CommandProcessors& aDependentCommandProcessors)
    : CommandProcessor(aName, aDependentCommandProcessors)
{
    assert(aDependentCommandProcessors.size() != 0);
}

void InputProcessor::ProcessLine(const std::string& aLine)
{
    if (aLine == "{")
    {
        if (mBlockDepth++ == 0)
        {
#ifdef DEBUG_PRINT
            std::cout << "InputProcessor::ProcessLine, startBlock, line = " << aLine << std::endl;
#endif
            CommandProcessor::StartBlock();
        }
    }
    else if (aLine == "}")
    {
        if (--mBlockDepth == 0)
        {
#ifdef DEBUG_PRINT
            std::cout << "InputProcessor::ProcessLine, finishBlock, line = " << aLine << std::endl;
#endif
            CommandProcessor::FinishBlock();
        }
    }
    else
    {
        Command command{aLine, std::chrono::system_clock::now()};
#ifdef DEBUG_PRINT
        std::cout << "InputProcessor::ProcessCommand, line = \'" << aLine << "\', length = " << aLine.length() << std::endl;
#endif
        CommandProcessor::ProcessCommand(command);
    }
    CommandProcessor::ProcessLine(aLine);
}
