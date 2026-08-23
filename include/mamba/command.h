#ifndef MAMBA_COMMAND_H
#define MAMBA_COMMAND_H

#include <string>
#include <map>
#include <functional>

namespace mamba {

class Command {
public:
    explicit Command(
        std::string name,
        std::string description,
        std::function<void()> action
    );
    ~Command() = default;

    void Execute();

    const std::string& name() const { return name_; }
    const std::string& description() const { return description_; }

private:
    std::string name_;
    std::string description_;
    std::function<void()> action_;
};

class CommandRegistry {
public:
    static CommandRegistry& Instance();

    void AddCommand(const Command& command);
    Command* GetCommand(const std::string& name);
    bool Execute(const std::string& name);

private:
    std::map<std::string, Command> commands_;
};

} // namespace mamba

#endif // MAMBA_COMMAND_H