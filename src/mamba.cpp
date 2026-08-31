#include "mamba/mamba.h"

#include <iostream>
#include <iomanip>

namespace mamba {

Mamba& Mamba::Instance() {
    static Mamba instance;
    return instance;
}

void Mamba::AddCommand(const Command& command) {
    Command cmd = command;
    cmd.AddFlag("--help", "-h", "Show help");
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
        cmd->PrintHelp();
        return;
    }
    cmd->Execute(args);
}

void Mamba::PrintHelp() const {
    std::cout << "\nUsage:\n";
    std::cout << "  " << app_name_ << " [flags]\n";
    std::cout << "  " << app_name_ << " [command]\n\n";
    std::cout << "Available Commands:\n";
    for (const auto& [name, cmd] : commands_) {
        std::cout << "  " << std::setw(12) << std::left << name << cmd.description() << "\n";
    }
    std::cout << "\nFlags:\n";
    std::cout << "  -h, --help    help for " << app_name_ << "\n";
    
    auto alias_it = aliases_.find("--version");
    if (alias_it != aliases_.end() && alias_it->second == "version") {
        auto cmd_it = commands_.find("version");
        if (cmd_it != commands_.end()) {
            std::cout << "  -v, --version   " << cmd_it->second.description() << "\n";
        }
    }
    
    std::cout << "\nUse \"" << app_name_ << " [command] --help\" for more information about a command.\n";
}

void Mamba::SetAppName(const std::string& name) {
    app_name_ = name;
}

void Mamba::EnsureHelpCommand() {
    if (commands_.count("help")) return;

    Command help_cmd(
        "help",
        "Help about any command",
        [this](const Command::ParsedArgs& args) {
            const auto& pos = args.positional();
            if (pos.empty()) {
                PrintHelp();
                return;
            }
            std::string cmd_name = pos[0];
            auto* cmd = GetCommand(cmd_name);
            if (!cmd) {
                std::cerr << "Unknown command: " << cmd_name << "\n";
                return;
            }
            cmd->PrintHelp();
        }
    );
    AddCommand(help_cmd);
}

void Mamba::Run(int argc, char* argv[]) {
    if (argc > 0 && app_name_.empty()) {
        app_name_ = argv[0];
        auto pos = app_name_.find_last_of("\\/");
        if (pos != std::string::npos) {
            app_name_ = app_name_.substr(pos + 1);
        }
        if (app_name_.size() > 4 && app_name_.substr(app_name_.size() - 4) == ".exe") {
            app_name_ = app_name_.substr(0, app_name_.size() - 4);
        }
    }

    EnsureHelpCommand();

    if (argc < 2) {
        PrintHelp();
        return;
    }

    std::string cmd = argv[1];
    if (cmd == "--help" || cmd == "-h") {
        PrintHelp();
        return;
    }

    std::vector<std::string> args(argv + 2, argv + argc);
    Execute(cmd, args);
}

}  // namespace mamba