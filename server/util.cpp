#include "util.hpp"

auto da::split_to(std::vector<std::string>& result, std::string const& str, std::string const& sep) -> void
{
    std::string token{};

    for(char const ch : str) {
        if(sep.find(ch) != std::string::npos) {
            result.push_back(token);
            token.clear();
        }
        else {
            token.push_back(ch);
        }
    }

    if(!token.empty()) {
        result.push_back(token);
    }
}

auto da::split(std::string const& str, std::string const& sep) -> std::vector<std::string>
{
    std::vector<std::string> tokens{};
    split_to(tokens, str, sep);
    return tokens;
}
