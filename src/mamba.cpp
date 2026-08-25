#include "mamba/mamba.h"

#include <iostream>
#include <vector>

namespace mamba {

Mamba& Mamba::Instance() {
    static Mamba instance;
    return instance;
}

void Mamba::AddCommand(const Command& command) {
    Command cmd = command;
    cmd.AddFlag("-h");
    cmd.AddFlag("--help");
    commands_.emplace(cmd.name(), std::move(cmd));
    for (const auto& alias : command.aliases()) {
        aliases_[alias] = command.name();
    }
}
    
Command* Mamba::GetCommand(const std::string& name) {
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

void Mamba::Describe(const std::string& name) {
    auto cmd = GetCommand(name);
    if (!cmd) {
        std::cerr << "Command not found: " << name << std::endl;
        return;
    }
    std::cout << cmd->description() << std::endl;
}

void Mamba::Execute(const std::string& name, const std::vector<std::string>& args) {
    auto cmd = GetCommand(name);
    if (!cmd) {
        std::cerr << "Command not found: " << name << std::endl;
        return;
    }

    bool show_help = false;
    for (const auto& a : args) {
        if (a == "-h" || a == "--help") {
            show_help = true;
            break;
        }
    }

    if (show_help) {
        std::cout << name << " - " << cmd->description() << std::endl;
        return;
    }
    cmd->Execute();
}

}  // namespace mamba