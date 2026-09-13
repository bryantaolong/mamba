#include "mamba/command.h"

#include <iostream>
#include <iomanip>

namespace mamba {

namespace {
std::unordered_map<std::string, std::string> BuildShortToLongMap(const std::unordered_map<std::string, mamba::Command::OptionDef>& options) {
    std::unordered_map<std::string, std::string> short_to_long;
    for (const auto& [key, def] : options) {
        if (!def.short_name.empty()) {
            short_to_long.emplace(def.short_name, def.long_name);
        }
    }
    return short_to_long;
}
}  // namespace

Command::Command(std::string name, std::string description, std::function<int(const ParsedArgs& args)> action) {
    name_ = std::move(name);
    description_ = std::move(description);
    action_ = std::move(action);
}

std::string Command::ParsedArgs::ResolveKey(const std::unordered_map<std::string, std::string>& short_to_long, const std::string& key) {
    auto it = short_to_long.find(key);
    if (it != short_to_long.end()) {
        return it->second;
    }
    return key;
}

std::optional<std::string> Command::ParsedArgs::GetOption(const std::string& key, const std::optional<std::string>& default_val) const {
    auto resolved = ResolveKey(short_to_long_, key);
    auto it = options_.find(resolved);
    if (it != options_.end()) {
        return it->second;
    }
    auto def = defaults_.find(resolved);
    if (def != defaults_.end()) {
        return def->second;
    }
    return default_val;
}

bool Command::ParsedArgs::HasFlag(const std::string& flag) const {
    return flags_.count(ResolveKey(short_to_long_, flag)) > 0;
}

bool Command::ParsedArgs::HasOption(const std::string& key) const {
    return options_.count(ResolveKey(short_to_long_, key)) > 0;
}

void Command::AddFlag(const std::string& long_name, const std::string& short_name, const std::string& description) {
    OptionDef def{long_name, short_name, description, true};
    options_[long_name] = def;
    if (!short_name.empty()) {
        options_[short_name] = def;
    }
}

void Command::AddOption(const std::string& long_name, const std::string& short_name, const std::string& description, std::optional<std::string> default_val) {
    OptionDef def{long_name, short_name, description, false, default_val};
    options_[long_name] = def;
    if (!short_name.empty()) {
        options_[short_name] = def;
    }
}

int Command::Execute(const std::vector<std::string>& args) {
    if (!action_) {
        std::cerr << "No action defined for command: " << name_ << std::endl;
        return 1;
    }

    auto short_to_long = BuildShortToLongMap(options_);

    ParsedArgs parsed;
    parsed.short_to_long_ = short_to_long;
    for (size_t i = 0; i < args.size(); ++i) {
        auto raw_key = args[i];
        auto key = ParsedArgs::ResolveKey(short_to_long, raw_key);
        auto it = options_.find(key);
        if (it != options_.end()) {
            if (it->second.is_flag) {
                parsed.flags_.insert(it->second.long_name);
            } else {
                if (i + 1 < args.size()) {
                    ++i;
                    if (options_.count(ParsedArgs::ResolveKey(short_to_long, args[i]))) {
                        std::cerr << "Error: option " << raw_key << " requires a value\n";
                        return 1;
                    }
                    parsed.options_[it->second.long_name] = args[i];
                } else {
                    std::cerr << "Error: option " << raw_key << " requires a value\n";
                    return 1;
                }
            }
        } else {
            parsed.positional_.push_back(args[i]);
        }
    }

    for (const auto& req : required_) {
        std::string canonical = ParsedArgs::ResolveKey(short_to_long, req);
        auto req_it = options_.find(canonical);
        if (req_it != options_.end()) {
            canonical = req_it->second.long_name;
        }

        bool found = parsed.options_.count(canonical) > 0 || parsed.flags_.count(canonical) > 0;
        if (!found) {
            std::cerr << "Error: missing required option: " << req << "\n";
            return 1;
        }
    }

    for (const auto& [key, def] : options_) {
        if (def.default_val.has_value()) {
            parsed.defaults_[def.long_name] = *def.default_val;
        }
    }

    return action_(parsed);
}

void Command::PrintHelp() const {
    std::cout << description_ << "\n\n";
    std::cout << "Usage:\n";
    std::cout << "  " << name_ << " [flags]\n\n";

    std::unordered_set<std::string> printed;
    std::cout << "Flags:\n";
    for (const auto& [key, def] : options_) {
        if (printed.count(def.long_name)) continue;
        printed.insert(def.long_name);

        std::string names;
        if (!def.short_name.empty()) {
            names = def.short_name + ", " + def.long_name;
        } else {
            names = def.long_name;
        }

        std::cout << "  " << std::setw(22) << std::left << names << def.description;
        if (required_.count(def.long_name)) {
            std::cout << " [required]";
        }
        if (def.default_val.has_value()) {
            std::cout << " (default: " << *def.default_val << ")";
        }
        std::cout << "\n";
    }
}

} // namespace mamba
