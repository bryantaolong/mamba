#include "mamba/command.h"

#include <iostream>

namespace mamba {

Command::Command(std::string name, std::string description, std::function<void()> action) {
    name_ = name;
    description_ = description;
    action_ = action;
}

void Command::Execute() {
    if (!action_) {
        std::cerr << "No action defined for command: " << name_ << std::endl;
        return;
    }
    action_();
}

CommandRegistry& CommandRegistry::Instance() {
    static CommandRegistry instance;
    return instance;
}

void CommandRegistry::AddCommand(const Command& command) {
    commands_.emplace(command.name(), command);
    for (const auto& alias : command.aliases()) {
        aliases_[alias] = command.name();
    }
}
    
Command* CommandRegistry::GetCommand(const std::string& name) {
    auto it = commands_.find(name);
    if (it != commands_.end()) {
        return &(it->second);
    }
    auto alias_it = aliases_.find(name);
    if (alias_it != aliases_.end()) {
        return GetCommand(alias_it->second);
    }
    return nullptr;
}

void CommandRegistry::Describe(const std::string& name) {
    auto cmd = GetCommand(name);
    if (!cmd) {
        std::cerr << "Command not found: " << name << std::endl;
        return;
    }
    std::cout << cmd->description() << std::endl;
}

void CommandRegistry::Execute(const std::string& name) {
    auto cmd = GetCommand(name);
    if (!cmd) {
        std::cerr << "Command not found: " << name << std::endl;
        return;
    }
    cmd->Execute();
}

}  // namespace mamba