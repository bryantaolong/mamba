# mamba

A lightweight C++ commander library for building CLI applications.

## Features

- Declarative command registration with `AddOption`, `AddFlag`, `MarkAsRequired`
- Order-independent argument parsing (short and long options)
- Option default values (`AddOption` with default, shown as `(default: ...)` in help)
- Required option validation accepts both long and short names
- Missing-value detection when next token is another known option
- `GetOption` returns `std::optional<std::string>` — distinguishes "not set" from empty string
- `HasOption` to tell whether an option was explicitly provided
- Exit-code propagation: actions return `int`; `Mamba::Run` returns the same value for the process
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
        [](const mamba::Command::ParsedArgs& args) -> int {
            auto msg = args.GetOption("-m", args.GetOption("--message"));
            bool force = args.HasFlag("-f");
            const auto& files = args.positional();

            if (!msg || *msg.empty()) {
                std::cerr << "error: no -m message given\n";
                return 1;
            }
            std::cout << "message: " << *msg << "\n";
            std::cout << "force: " << (force ? "yes" : "no") << "\n";
            for (const auto& f : files) std::cout << " " << f;
            std::cout << "\n";
            return 0;
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
        [](const mamba::Command::ParsedArgs&) -> int {
            std::cout << "v1.0.0\n";
            return 0;
        }
    );
    version_cmd.AddAlias("--version");
    version_cmd.AddAlias("-v");
    mamba.AddCommand(version_cmd);

    mamba.SetAppName("pdfx");
    return mamba.Run(argc, argv);
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
| `Run(int argc, char* argv[])` | Parse argv and dispatch; returns action's exit code |
| `Execute(const std::string&, const std::vector<std::string>&)` | Dispatch a named command; returns int |
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
| `GetOption(key, default = nullopt)` | Get option value by long or short name; returns `std::optional<std::string>` |
| `HasOption(key)` | Whether the option was explicitly provided |
| `HasFlag(flag)` | Check if a flag is present |
| `positional()` | Remaining positional arguments |

## Build

```bash
cmake -B build
cmake --build build
```

## Test

```bash
ctest --test-dir build --output-on-failure
```

## Install

Installs the headers, static library, and CMake/pkg-config config files so other
projects can link against mamba via `find_package`.

```bash
cmake -B build
cmake --build build
# Install to a custom prefix (dry-run safe, does not touch system dirs)
cmake --install build --prefix ~/.local
# Or system-wide on Linux/macOS (headers -> /usr/include, lib -> /usr/lib)
sudo cmake --install build --prefix /
```

Use it in a consumer project:

```cmake
find_package(mamba 1.0 REQUIRED)
target_link_libraries(myapp PRIVATE mamba::mamba)
```

Or via pkg-config:

```bash
g++ main.cpp $(pkg-config --cflags --libs mamba) -o myapp
```

## License

MIT
