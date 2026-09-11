#include "mamba/command.h"
#include "mamba/mamba.h"

#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace {

int failures = 0;

void Assert(bool cond, const std::string& msg) {
    if (!cond) {
        std::cerr << "FAIL: " << msg << std::endl;
        ++failures;
    }
}

}  // namespace

int main() {
    // 1. Short option input normalizes to long name (query by long name)
    {
        mamba::Command cmd("add", "", [](const mamba::Command::ParsedArgs& args) {
            if (auto v = args.GetOption("--message")) {
                std::cout << *v << "\n";
            }
        });
        cmd.AddOption("--message", "-m", "msg");

        std::stringstream buf;
        std::streambuf* old = std::cout.rdbuf(buf.rdbuf());
        cmd.Execute({"-m", "hello"});
        std::cout.rdbuf(old);
        Assert(buf.str() == "hello\n", "short option should parse and query by long name");
    }

    // 2. Long option
    {
        mamba::Command cmd("add", "", [](const mamba::Command::ParsedArgs& args) {
            if (auto v = args.GetOption("--message")) {
                std::cout << *v << "\n";
            }
        });
        cmd.AddOption("--message", "-m", "msg");

        std::stringstream buf;
        std::streambuf* old = std::cout.rdbuf(buf.rdbuf());
        cmd.Execute({"--message", "world"});
        std::cout.rdbuf(old);
        Assert(buf.str() == "world\n", "long option should be parsed");
    }

    // 3. Flag parsing
    {
        bool flag = false;
        mamba::Command cmd("add", "", [&flag](const mamba::Command::ParsedArgs& args) {
            flag = args.HasFlag("--force");
        });
        cmd.AddFlag("--force", "-f", "force");

        cmd.Execute({"-f"});
        Assert(flag, "flag should be set");
    }

    // 4. Positional args
    {
        std::vector<std::string> pos;
        mamba::Command cmd("add", "", [&pos](const mamba::Command::ParsedArgs& args) {
            const auto& p = args.positional();
            pos.assign(p.begin(), p.end());
        });
        cmd.AddOption("--message", "-m", "msg");

        cmd.Execute({"-m", "msg", "file1", "file2"});
        Assert(pos.size() == 2 && pos[0] == "file1" && pos[1] == "file2", "positional args");
    }

    // 5. Required option missing
    {
        mamba::Command cmd("add", "", [](const mamba::Command::ParsedArgs&){});
        cmd.AddOption("--message", "-m", "msg");
        cmd.MarkAsRequired("--message");

        std::stringstream buf;
        std::streambuf* old = std::cerr.rdbuf(buf.rdbuf());
        cmd.Execute({});
        std::cerr.rdbuf(old);

        Assert(buf.str().find("missing required option") != std::string::npos,
               "should report missing required option");
    }

    // 6. Required option satisfied
    {
        mamba::Command cmd("add", "", [](const mamba::Command::ParsedArgs&){});
        cmd.AddOption("--message", "-m", "msg");
        cmd.MarkAsRequired("--message");

        std::stringstream buf;
        std::streambuf* old = std::cerr.rdbuf(buf.rdbuf());
        cmd.Execute({"-m", "msg"});
        std::cerr.rdbuf(old);

        Assert(buf.str().empty(), "required option satisfied, no error");
    }

    // 7. Aliases
    {
        mamba::Mamba& mamba = mamba::Mamba::Instance();
        bool hit = false;
        mamba::Command cmd("test", "", [&hit](const mamba::Command::ParsedArgs&) {
            hit = true;
        });
        cmd.AddAlias("--t");
        mamba.AddCommand(cmd);

        std::stringstream buf;
        std::streambuf* old = std::cout.rdbuf(buf.rdbuf());
        mamba.Execute("--t");
        std::cout.rdbuf(old);

        Assert(hit, "alias should resolve to command");
    }

    // 8. SetAppName
    {
        mamba::Mamba& mamba = mamba::Mamba::Instance();
        mamba.SetAppName("pdfx");

        std::stringstream buf;
        std::streambuf* old = std::cout.rdbuf(buf.rdbuf());
        mamba.PrintHelp();
        std::cout.rdbuf(old);

        Assert(buf.str().find("pdfx") != std::string::npos, "help should contain app name");
    }

    // 9. Missing value: next token is a known option
    {
        mamba::Command cmd("add", "", [](const mamba::Command::ParsedArgs&){});
        cmd.AddOption("--message", "-m", "msg");
        cmd.AddFlag("--force", "-f", "force");

        std::stringstream buf;
        std::streambuf* old = std::cerr.rdbuf(buf.rdbuf());
        cmd.Execute({"-m", "-f"});
        std::cerr.rdbuf(old);

        Assert(buf.str().find("requires a value") != std::string::npos,
               "should reject option followed by another known option");
    }

    // 10. Required check accepts short name registration
    {
        mamba::Command cmd("add", "", [](const mamba::Command::ParsedArgs&){});
        cmd.AddOption("--message", "-m", "msg");
        cmd.MarkAsRequired("-m");

        std::stringstream buf;
        std::streambuf* old = std::cerr.rdbuf(buf.rdbuf());
        cmd.Execute({"-m", "msg"});
        std::cerr.rdbuf(old);

        Assert(buf.str().empty(), "required check should accept short registered name");
    }

    if (failures == 0) {
        std::cout << "All tests passed.\n";
        return 0;
    }
    return 1;
}
