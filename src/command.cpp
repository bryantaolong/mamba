#include "mamba/command.h"

namespace mamba {

Command::Command(std::string name, std::string description, std::function<void()> action) {
    name_ = name;
    description_ = description;
    action_ = action;
}

void Command::Execute() {
    if (action_) action_();
}

CommandRegistry& CommandRegistry::Instance() {
    static CommandRegistry instance;
    return instance;
}

void CommandRegistry::AddCommand(const Command& command) {
    commands_.emplace(command.name(), command);
}
    
Command* CommandRegistry::GetCommand(const std::string& name) {
    auto it = commands_.find(name);
    if (it != commands_.end()) {
        return &(it->second);
    }
    return nullptr;
}

bool CommandRegistry::Execute(const std::string& name) {
    auto it = commands_.find(name);
    if (it == commands_.end()) {
        return false;
    }
    it->second.Execute();
    return true;
}

}  // namespace mamba