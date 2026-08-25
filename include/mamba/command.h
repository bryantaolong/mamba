#ifndef MAMBA_COMMAND_H
#define MAMBA_COMMAND_H

#include <string>
#include <map>
#include <unordered_set>
#include <unordered_map>
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
    void AddFlag(const std::string& flag) { flags_.insert(flag); }
    void AddAlias(const std::string& alias) { aliases_.insert(alias); }

    const std::string& name() const { return name_; }
    const std::string& description() const { return description_; }
    const std::unordered_set<std::string>& aliases() const { return aliases_; }

private:
    std::string name_;
    std::string description_;
    std::unordered_set<std::string> flags_;
    std::unordered_set<std::string> aliases_;
    std::function<void()> action_;
};

class CommandRegistry {
public:
    static CommandRegistry& Instance();

    void AddCommand(const Command& command);
    Command* GetCommand(const std::string& name);
    void Describe(const std::string& name);
    void Execute(const std::string& name);

private:
    // std::map keeps keys ordered, which is useful for help/usage output later
    std::map<std::string, Command> commands_;
    // std::unordered_map for O(1) alias lookup; ordering is not needed here
    std::unordered_map<std::string, std::string> aliases_;
};

} // namespace mamba

#endif // MAMBA_COMMAND_H