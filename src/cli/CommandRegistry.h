#pragma once

#include <Arduino.h>
#include <vector>
#include "ICommandInterface.h"

struct HelpTopic
{
    String topic;
    void (*helpFunction)(ICommandInterface&);
};

class CommandRegistry
{
public:
    static void registerHelp(const String& topic, void (*helpFunction)(ICommandInterface&));
    static bool printHelp(const String &topic, ICommandInterface &io);
    static void printTopics(ICommandInterface& io);

private:
    static std::vector<HelpTopic> topics;
};
