#ifndef MAMBA_MAMBA_H
#define MAMBA_MAMBA_H

#include <string>
#include <map>
#include <unordered_map>
#include <vector>

#include "mamba/command.h"

namespace mamba {

class Mamba {
public:
    static Mamba& Instance();

    void AddCommand(const Command& command);
    Command* GetCommand(const std::string& name);
    void Describe(const std::string& name);
    void Execute(const std::string& name, const std::vector<std::string>& args = {});

private:
    // std::map keeps keys ordered, which is useful for help/usage output later
    std::map<std::string, Command> commands_;
    // std::unordered_map for O(1) alias lookup; ordering is not needed here
    std::unordered_map<std::string, std::string> aliases_;
};

} // namespace mamba

#endif // MAMBA_MAMBA_H