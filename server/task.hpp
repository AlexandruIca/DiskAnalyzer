#ifndef TASK_HPP
#define TASK_HPP
#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

namespace da {

///
/// Thread id or task id, whatever you want it to be
///
using tid_t = std::int64_t;

[[maybe_unused]] constexpr tid_t no_task = 0;

enum class task_priority : std::size_t
{
    low = 0,
    medium,
    high,
    count
};

struct thread_state
{
    bool pause = false;
    bool stop = false;
};

struct file_properties
{
    std::string name{};
    std::size_t size_in_bytes = 0;
};

struct worker_state
{
    tid_t task_id = no_task;
    std::size_t total_size = 0;
    std::vector<file_properties> progress{};
};

} // namespace da

#endif // !TASK_HPP
