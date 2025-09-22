// format_utils.h
#pragma once
#include <string>
#include <vector>
#include <sstream>
template<typename T>
std::string to_string_helper(T&& arg) {
    std::ostringstream oss;
    oss << std::forward<T>(arg);
    return oss.str();
}

template<typename... Args>
std::string formatMessage(const std::string& format, Args&&... args) {
    std::vector<std::string> arg_strings = { to_string_helper(std::forward<Args>(args))... };
    std::ostringstream oss;
    size_t arg_index = 0;
    size_t pos = 0;
    size_t placeholder = format.find("{}", pos);


    while (placeholder != std::string::npos) {
        oss << format.substr(pos, placeholder - pos);
        if (arg_index < arg_strings.size()) {
            oss << arg_strings[arg_index++];
        } else {
            oss << "{}";
        }
        pos = placeholder + 2;
        placeholder = format.find("{}", pos);
    }
    oss << format.substr(pos);


    while (arg_index < arg_strings.size()) {
        oss << arg_strings[arg_index++];
    }


    return oss.str();
}