#ifndef ASSERT_HPP
#define ASSERT_HPP
#pragma once

#include <cstdlib>

#include "config.hpp"
#include "log.hpp"

#define ASSERT(...)                                                                                                    \
    if(!(__VA_ARGS__)) {                                                                                               \
        ERROR("Asserton failed at %s[%d]\n", __FILE__, __LINE__);                                                      \
        std::exit(exit_kind::failure);                                                                                 \
    }                                                                                                                  \
    static_cast<void>(0)

#define ASSERT_MSG(cnd, msg)                                                                                           \
    if(!(cnd)) {                                                                                                       \
        ERROR("Asserton failed at %s[%d]\n\t:%s\n", __FILE__, __LINE__, msg);                                          \
        std::exit(exit_kind::failure);                                                                                 \
    }                                                                                                                  \
    static_cast<void>(0)

#endif // !ASSERT_HPP
