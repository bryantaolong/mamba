#include <iostream>

#include "mamba/mamba.h"
#include "mamba/command.h"

int main(int argc, char* argv[]) {
    mamba::Mamba& mamba = mamba::Mamba::Instance();

    mamba::Command version_cmd(
        "version",
        "Show version information",
        [](const mamba::Command::ParsedArgs& args) { std::cout << "v1.0.0\n"; }
    );

    version_cmd.AddAlias("--version");
    version_cmd.AddAlias("-v");
    mamba.AddCommand(version_cmd);

    mamba::Command add_cmd(
        "add",
        "Add file contents to the index",
        [](const mamba::Command::ParsedArgs& args) {
            std::string msg = args.GetOption("-m", args.GetOption("--message"));
            bool force = args.HasFlag("-f") || args.HasFlag("--force");
            const auto& files = args.positional();

            if (msg.empty()) {
                std::cerr << "error: no -m message given\n";
                return;
            }
            std::cout << "message: " << msg << "\n";
            std::cout << "force: " << (force ? "yes" : "no") << "\n";
            std::cout << "files:";
            for (const auto& f : files) {
                std::cout << " " << f;
            }
            std::cout << "\n";
        }
    );
    add_cmd.AddOption("--message", "-m", "Commit message");
    add_cmd.MarkAsRequired("--message");
    add_cmd.AddFlag("--force", "-f", "Force add");
    add_cmd.AddAliases({"--add", "-a"});
    mamba.AddCommand(add_cmd);
    
    mamba.SetAppName("pdfx");
    mamba.Run(argc, argv);
    return 0;
}
