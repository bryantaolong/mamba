#include "mamba/command.h"

#include <iostream>

int main(int argc, char* argv[]) {
    mamba::CommandRegistry& registry = mamba::CommandRegistry::Instance();

    registry.AddCommand(mamba::Command(
        "version",
        "Show version information",
        []() { std::cout << "v1.0.0\n"; }
    ));

    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <command>\n";
        std::cerr << "Try '" << argv[0] << " version' for version info.\n";
        return 1;
    }

    std::string arg = argv[1];
    if (arg == "--version" || arg == "-v") {
        arg = "version";
    }

    if (!registry.Execute(arg)) {
        std::cerr << "Unknown command: " << arg << "\n";
        return 1;
    }

    return 0;
}
