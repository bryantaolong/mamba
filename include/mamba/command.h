#ifndef MAMBA_COMMAND_H
#define MAMBA_COMMAND_H

#include <string>
#include <unordered_set>
#include <unordered_map>
#include <functional>
#include <vector>

namespace mamba {

class Command {
public:
    struct OptionDef {
        std::string long_name;
        std::string short_name;
        std::string description;
        bool is_flag = false;
    };

    struct ParsedArgs {
        std::string GetOption(const std::string& key, const std::string& default_val = "") const;
        bool HasFlag(const std::string& flag) const;
        const std::vector<std::string>& positional() const { return positional_; }

    private:
        std::unordered_map<std::string, std::string> options_;
        std::unordered_set<std::string> flags_;
        std::vector<std::string> positional_;
        friend class Command;
    };

    explicit Command(
        std::string name,
        std::string description,
        std::function<void(const ParsedArgs& args)> action
    );
    ~Command() = default;

    void Execute(const std::vector<std::string>& args);
    void PrintHelp() const;
    void AddFlag(const std::string& long_name, const std::string& short_name = "", const std::string& description = "");
    void AddOption(const std::string& long_name, const std::string& short_name = "", const std::string& description = "");
    void MarkAsRequired(const std::string& long_name) { required_.insert(long_name); }
    void AddAlias(const std::string& alias) { aliases_.insert(alias); }
    void AddAliases(std::initializer_list<std::string> aliases) {
        for (const auto& alias : aliases) {
            aliases_.insert(alias);
        }
    }

    const std::string& name() const { return name_; }
    const std::string& description() const { return description_; }
    const std::unordered_set<std::string>& aliases() const { return aliases_; }

private:
    std::string name_;
    std::string description_;
    std::unordered_set<std::string> aliases_;
    std::unordered_map<std::string, OptionDef> options_;
    std::unordered_set<std::string> required_;
    std::function<void(const ParsedArgs& args)> action_;
};

} // namespace mamba

#endif // MAMBA_COMMAND_H