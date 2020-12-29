#ifndef UTIL_HPP
#define UTIL_HPP
#pragma once

#include <string>
#include <vector>

namespace da {

auto split_to(std::vector<std::string>& result, std::string const& str, std::string const& sep) -> void;
auto split(std::string const& str, std::string const& sep) -> std::vector<std::string>;
/*
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
*/

} // namespace da

#endif // !UTIL_HPP
