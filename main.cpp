#include "mamba/command.h"

#include <iostream>

class VersionCommand : public mamba::Command {
public:
    VersionCommand() : mamba::Command(
        "version",
        "Show version information",
        []() { std::cout << "mamba version 1.0.0\n"; }
    ) {}
};

int main() {
    mamba::CommandRegistry& registry = mamba::CommandRegistry::instance();

    VersionCommand version_cmd;

    registry.add_command(version_cmd);

    registry.execute("version");
    return 0;
}
