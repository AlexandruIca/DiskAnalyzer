#include "filesystem.hpp"
#include "task.hpp"

#include <iostream>
#include <stdexcept>
#include <utility>

auto da::to_path(std::string const& from) -> path_t
{
    path_t path = split(from, "/ ");

    auto begin = std::remove_if(path.begin(), path.end(), [](std::string const& str) { return str.empty(); });
    path.erase(begin, path.end());

    return path;
}

namespace da {

auto fs_tree::print_indentation(std::ostream& os, int const indent) -> void
{
    for(int i = 0; i < indent - 1; ++i) {
        os << '-';
    }

    os << '>' << ' ';
}

auto fs_tree::pretty_print_to_impl(std::ostream& os, int const indent, ptr_t const ptr, std::string const& dir_name)
    -> void
{
    if(ptr == nullptr) {
        return;
    }

    print_indentation(os, indent);
    os << (dir_name.empty() ? "root" : dir_name) << '\n';

    for(auto const& [key, val] : ptr->get_dirs()) {
        pretty_print_to_impl(os, indent + s_indent_default, val.get(), key);
    }
}

auto fs_tree::add(path_t const& path) -> void
{
    std::scoped_lock<std::mutex> lock{ m_mutex };
    ptr_t ptr = &m_tree;

    for(auto const& segment : path) {
        if(ptr->contains_dir(segment)) {
            ptr = (*ptr)[segment].get();
        }
        else {
            ptr = ptr->insert_dir(segment).get();
        }
    }
}

auto fs_tree::at(path_t const& path) const -> worker_state
{
    std::scoped_lock<std::mutex> lock{ m_mutex };
    const_ptr_t ptr = &m_tree;

    for(auto const& segment : path) {
        if(ptr->contains_dir(segment)) {
            ptr = (*ptr)[segment].get();
        }
        else {
            throw std::runtime_error{ "Trying to access a non-existent directory! [::da::fs_tree::at]" };
        }
    }

    return ptr->data();
}

auto fs_tree::set_at(path_t const& path, worker_state state) -> void
{
    std::scoped_lock<std::mutex> lock{ m_mutex };
    ptr_t ptr = &m_tree;

    for(auto const& segment : path) {
        if(ptr->contains_dir(segment)) {
            ptr = (*ptr)[segment].get();
        }
        else {
            throw std::runtime_error{ "Trying to access a non-existent directory! [::da::fs_tree::set_at]" };
        }
    }

    ptr->set_data(std::move(state));
}

auto fs_tree::is_already_being_processed(path_t const& path) const -> bool
{
    std::scoped_lock<std::mutex> lock{ m_mutex };
    const_ptr_t ptr = &m_tree;

    for(auto const& segment : path) {
        if(ptr->contains_dir(segment)) {
            ptr = (*ptr)[segment].get();

            if(ptr->data().task_id != da::no_task) {
                return true;
            }
        }
        else {
            return false;
        }
    }

    return ptr->data().task_id != da::no_task;
}

auto fs_tree::exists(path_t const& path) const noexcept -> bool
{
    std::scoped_lock<std::mutex> lock{ m_mutex };
    const_ptr_t ptr = &m_tree;

    for(auto const& segment : path) {
        if(ptr->contains_dir(segment)) {
            ptr = (*ptr)[segment].get();
        }
        else {
            return false;
        }
    }

    return true;
}

auto fs_tree::pretty_print_to(std::ostream& os, int const indent) -> void
{
    std::scoped_lock<std::mutex> lock{ m_mutex };
    this->pretty_print_to_impl(os, indent, &m_tree, "");
}

} // namespace da
