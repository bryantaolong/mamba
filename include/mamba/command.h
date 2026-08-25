#ifndef MAMBA_COMMAND_H
#define MAMBA_COMMAND_H

#include <string>
#include <unordered_set>
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
    std::unordered_set<std::string> aliases_;
    std::unordered_set<std::string> flags_;
    std::function<void()> action_;
};

} // namespace mamba

#endif // MAMBA_COMMAND_H