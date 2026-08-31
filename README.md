# mamba

A lightweight C++ commander library for building CLI applications.

## Features

- Declarative command registration with `AddOption`, `AddFlag`, `MarkAsRequired`
- Order-independent argument parsing (short and long options)
- Auto-generated `--help` / `-h` for every command
- Auto-generated top-level help and `help [command]` subcommand
- Command aliases with `AddAlias` and bulk `AddAliases`
- `SetAppName` to decouple display name from binary filename

## Quick Start

```cpp
#include "mamba/mamba.h"
#include "mamba/command.h"

int main(int argc, char* argv[]) {
    mamba::Mamba& mamba = mamba::Mamba::Instance();

    mamba::Command add_cmd(
        "add",
        "Add file contents to the index",
        [](const mamba::Command::ParsedArgs& args) {
            std::string msg = args.GetOption("-m", args.GetOption("--message"));
            bool force = args.HasFlag("-f");
            const auto& files = args.positional();

            if (msg.empty()) {
                std::cerr << "error: no -m message given\n";
                return;
            }
            std::cout << "message: " << msg << "\n";
            std::cout << "force: " << (force ? "yes" : "no") << "\n";
            for (const auto& f : files) std::cout << " " << f;
            std::cout << "\n";
        }
    );

    add_cmd.AddOption("--message", "-m", "Commit message");
    add_cmd.MarkAsRequired("--message");
    add_cmd.AddFlag("--force", "-f", "Force add");
    add_cmd.AddAliases({"--add", "-a"});
    mamba.AddCommand(add_cmd);

    mamba::Command version_cmd(
        "version",
        "Show version information",
        [](const mamba::Command::ParsedArgs&) { std::cout << "v1.0.0\n"; }
    );
    version_cmd.AddAlias("--version");
    version_cmd.AddAlias("-v");
    mamba.AddCommand(version_cmd);

    mamba.SetAppName("pdfx");
    mamba.Run(argc, argv);
}
```

## Usage

```bash
# Top-level help
$ pdfx --help

# Subcommand help
$ pdfx add --help

# Execute with mixed argument order
$ pdfx --add -m "fix bug" -f file1 file2
$ pdfx add file1 -m "fix bug" --force
```

## API

### Mamba

| Method | Description |
|---|---|
| `AddCommand(const Command&)` | Register a command |
| `Run(int argc, char* argv[])` | Parse argv and dispatch |
| `PrintHelp() const` | Print top-level help |
| `SetAppName(const std::string&)` | Set display name for help output |

### Command

| Method | Description |
|---|---|
| `AddOption(long, short, desc)` | Register an option that takes a value |
| `AddFlag(long, short, desc)` | Register a boolean flag |
| `MarkAsRequired(long)` | Mark an option as required |
| `AddAlias(str)` / `AddAliases({...})` | Register command aliases |
| `PrintHelp() const` | Print command-specific help |

### ParsedArgs

| Method | Description |
|---|---|
| `GetOption(key, default)` | Get option value by long or short name |
| `HasFlag(flag)` | Check if a flag is present |
| `positional()` | Get remaining positional arguments |

## Build

```bash
cmake -B build
cmake --build build
```

## Test

```bash
ctest --test-dir build --output-on-failure
```

## License

MIT
