#include "task_manager.hpp"

#include <chrono>
#include <iostream>
#include <thread>
#include <utility>

namespace da {

auto task_manager::start_worker(task_priority const p, tid_t const id) -> void
{
    using namespace std::chrono_literals;
    auto const priority = static_cast<std::size_t>(p);

    while(!m_threads.at(priority)[id].stop) {
        if(m_threads.at(priority)[id].pause) {
            std::this_thread::sleep_for(15ms);
            continue;
        }

        std::cout << id << std::endl;
        std::this_thread::sleep_for(250ms);
    }

    m_threads.at(priority).remove(id);
}

task_manager::~task_manager() noexcept
{
    for(auto const p : { task_priority::low, task_priority::medium, task_priority::high }) {
        for(auto& [id, _] : m_threads.at(std::size_t(p))) {
            this->stop_task(p, id);
        }
    }
}

auto task_manager::resume_task(task_priority const p, tid_t const id) -> void
{
    auto const priority = static_cast<std::size_t>(p);
    auto state = m_threads.at(priority)[id];

    state.pause = false;
    m_threads.at(priority).insert_or_update(id, std::forward<thread_state>(state));
}

auto task_manager::stop_task(task_priority const p, tid_t const id) -> void
{
    auto const priority = static_cast<std::size_t>(p);
    auto state = m_threads.at(priority)[id];

    state.stop = true;
    m_threads.at(priority).insert_or_update(id, std::forward<thread_state>(state));

    if((m_threads.at(priority).size() == 1 || m_threads.at(priority).empty()) && priority > 0) {
        for(auto const [id_inner, _] : m_threads.at(priority - 1)) {
            this->resume_task(task_priority{ priority - 1 }, id_inner);
        }
    }
}

auto task_manager::pause_task(task_priority const p, tid_t const id) -> void
{
    auto const priority = static_cast<std::size_t>(p);
    auto state = m_threads.at(priority)[id];

    state.pause = true;
    m_threads.at(priority).insert_or_update(id, std::forward<thread_state>(state));
}

auto task_manager::add_task(task_priority const p, tid_t const id, thread_state state) -> void
{
    auto const priority = static_cast<std::size_t>(p);

    if(p != task_priority::high) {
        for(std::size_t i = priority + 1; i < std::size_t(task_priority::count); ++i) {
            if(!m_threads.at(i).empty()) {
                state.pause = true;
                break;
            }
        }
    }

    if(!m_threads.at(priority).contains(id)) {
        if(m_threads.at(priority).empty()) {
            for(std::size_t i = 0; i < priority; ++i) {
                for(auto const [id_inner, _] : m_threads.at(i)) {
                    this->pause_task(task_priority{ i }, id_inner);
                }
            }
        }

        m_threads.at(priority).insert_or_update(id, std::forward<thread_state>(state));
        m_pool.push([this, p, id] { this->start_worker(p, id); });
    }
}

} // namespace da
