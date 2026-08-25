#include "mamba/command.h"

#include <iostream>

int main(int argc, char* argv[]) {
    mamba::CommandRegistry& registry = mamba::CommandRegistry::Instance();

    mamba::Command version_cmd(
        "version",
        "Show version information",
        []() { std::cout << "v1.0.0\n"; }
    );
    version_cmd.AddAlias("--version");
    version_cmd.AddAlias("-v");
    registry.AddCommand(version_cmd);

    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <command>\n";
        std::cerr << "Try '" << argv[0] << " version' for version info.\n";
        return 1;
    }

    std::string arg = argv[1];

    registry.Describe(arg);
    registry.Execute(arg);

    return 0;
}
