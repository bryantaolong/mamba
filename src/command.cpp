#include "mamba/command.h"

#include <iostream>
#include <iomanip>

namespace mamba {

Command::Command(std::string name, std::string description, std::function<void(const ParsedArgs& args)> action) {
    name_ = name;
    description_ = description;
    action_ = action;
}

std::string Command::ParsedArgs::GetOption(const std::string& key, const std::string& default_val) const {
    auto it = options_.find(key);
    return it != options_.end() ? it->second : default_val;
}

bool Command::ParsedArgs::HasFlag(const std::string& flag) const {
    return flags_.count(flag) > 0;
}

void Command::AddFlag(const std::string& long_name, const std::string& short_name, const std::string& description) {
    OptionDef def{long_name, short_name, description, true};
    options_[long_name] = def;
    if (!short_name.empty()) {
        options_[short_name] = def;
    }
}

void Command::AddOption(const std::string& long_name, const std::string& short_name, const std::string& description) {
    OptionDef def{long_name, short_name, description, false};
    options_[long_name] = def;
    if (!short_name.empty()) {
        options_[short_name] = def;
    }
}

void Command::Execute(const std::vector<std::string>& args) {
    if (!action_) {
        std::cerr << "No action defined for command: " << name_ << std::endl;
        return;
    }

    ParsedArgs parsed;
    for (size_t i = 0; i < args.size(); ++i) {
        auto it = options_.find(args[i]);
        if (it != options_.end()) {
            if (it->second.is_flag) {
                parsed.flags_.insert(args[i]);
            } else {
                if (i + 1 < args.size()) {
                    std::string key = args[i];
                    ++i;
                    parsed.options_[std::move(key)] = args[i];
                } else {
                    std::cerr << "Error: option " << args[i] << " requires a value\n";
                    return;
                }
            }
        } else {
            parsed.positional_.push_back(args[i]);
        }
    }

    for (const auto& req : required_) {
        bool found = false;
        for (const auto& [k, v] : parsed.options_) {
            auto it2 = options_.find(k);
            if (it2 != options_.end() && it2->second.long_name == req) {
                found = true;
                break;
            }
        }
        if (!found) {
            for (const auto& f : parsed.flags_) {
                auto it2 = options_.find(f);
                if (it2 != options_.end() && it2->second.long_name == req) {
                    found = true;
                    break;
                }
            }
        }
        if (!found) {
            std::cerr << "Error: missing required option: " << req << "\n";
            return;
        }
    }

    action_(parsed);
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
        std::cout << "\n";
    }
}

}  // namespace mamba
