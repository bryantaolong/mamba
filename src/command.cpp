#include "mamba/command.h"

namespace mamba {

Command::Command(std::string name, std::string description, std::function<void()> action) {
    name_ = name;
    description_ = description;
    action_ = action;
}

void Command::execute() {
    if (action_) action_();
}

CommandRegistry& CommandRegistry::instance() {
    static CommandRegistry instance;
    return instance;
}

void CommandRegistry::add_command(const Command& command) {
    commands_.emplace(command.name(), command);
}
    
Command* CommandRegistry::get_command(const std::string& name) {
    auto it = commands_.find(name);
    if (it != commands_.end()) {
        return &(it->second);
    }
    return nullptr;
}

void CommandRegistry::execute(const std::string& name) {
    auto it = commands_.find(name);
    if (it != commands_.end()) {
        it->second.execute();
    }
}

}  // namespace mamba