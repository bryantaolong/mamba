#ifndef COMMAND_H
#define COMMAND_H

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

    void execute();

    const std::string& name() const { return name_; }
    const std::string& description() const { return description_; }

private:
    std::string name_;
    std::string description_;
    std::function<void()> action_;
};

class CommandRegistry {
public:
    static CommandRegistry& instance();

    void add_command(const Command& command);
    Command* get_command(const std::string& name);
    void execute(const std::string& name);

private:
    std::map<std::string, Command> commands_;
};

} // namespace mamba

#endif // COMMAND_H