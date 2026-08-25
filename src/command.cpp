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

}  // namespace mamba