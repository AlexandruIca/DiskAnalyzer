#ifndef TASK_MANAGER_HPP
#define TASK_MANAGER_HPP
#pragma once

#include <array>

#include "task.hpp"
#include "thread_pool.hpp"
#include "unordered_map.hpp"

namespace da {

struct task_manager
{
private:
    static constexpr auto s_num_priorities = int(task_priority::count);
    std::array<unordered_map<tid_t, thread_state>, s_num_priorities> m_threads{};
    thread_pool m_pool{};

    auto start_worker(task_priority p, tid_t id) -> void;

public:
    task_manager() = default;
    task_manager(task_manager const&) = delete;
    task_manager(task_manager&&) = delete;
    ~task_manager() noexcept;

    auto operator=(task_manager const&) -> task_manager& = delete;
    auto operator=(task_manager &&) -> task_manager& = delete;

    auto resume_task(task_priority p, tid_t id) -> void;
    auto stop_task(task_priority p, tid_t id) -> void;
    auto pause_task(task_priority p, tid_t id) -> void;
    auto add_task(task_priority p, tid_t id, thread_state state = thread_state{}) -> void;
};

} // namespace da

#endif // !TASK_MANAGER_HPP
