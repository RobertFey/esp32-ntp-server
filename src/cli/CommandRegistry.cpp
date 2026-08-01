#include "CommandRegistry.h"

std::vector<HelpTopic> CommandRegistry::topics;

void CommandRegistry::registerHelp(const String& topic, void (*helpFunction)(ICommandInterface&))
{
    topics.push_back(
        {topic, helpFunction}
    );
}

bool CommandRegistry::printHelp(const String& topic, ICommandInterface& io)
{
    for (auto& t : topics)
    {
        if (t.topic == topic)
        {
            t.helpFunction(io);
            return true;
        }
    }

    return false;
}

void CommandRegistry::printTopics(ICommandInterface& io)
{
    io.println("Available help topics:");
    io.println("");
    for (auto& t : topics)
    {
       io.println("  " + t.topic);
    }

    io.println("");
    io.print("Use: help <topic>");
}
