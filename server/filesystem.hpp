#ifndef FILESYSTEM_HPP
#define FILESYSTEM_HPP
#pragma once

#include <algorithm>
#include <iosfwd>
#include <memory>
#include <mutex>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <vector>

#include "task.hpp"
#include "util.hpp"

namespace da {

template<typename T>
class dir_node
{
    static_assert(std::is_default_constructible_v<T>, "The data type for `dir_node` must have a default constructor!");

private:
    using dirs_t = std::unordered_map<std::string, std::unique_ptr<dir_node>>;

    T m_data{};
    dirs_t m_dirs{};

public:
    dir_node() = default;
    dir_node(dir_node const&) = default;
    dir_node(dir_node&&) noexcept = default;
    ~dir_node() noexcept = default;

    explicit dir_node(T&& val)
        : m_data{ std::forward<T>(val) }
    {
    }

    auto operator=(dir_node const&) -> dir_node& = default;
    auto operator=(dir_node&&) noexcept -> dir_node& = default;

    auto insert_dir(std::string const& key, T&& value = T{}) -> std::unique_ptr<dir_node>&
    {
        if(m_dirs.find(key) != m_dirs.end()) {
            throw std::runtime_error{ "Trying to insert an already existing directory!" };
        }

        m_dirs.emplace(key, std::make_unique<dir_node>(std::forward<T>(value)));
        return m_dirs.at(key);
    }

    auto remove_dir(std::string const& key) -> void
    {
        if(auto const it = m_dirs.find(key); it != m_dirs.end()) {
            m_dirs.erase(it);
        }
    }

    [[nodiscard]] auto operator[](std::string const& key) -> std::unique_ptr<dir_node>&
    {
        return m_dirs.at(key);
    }

    [[nodiscard]] auto operator[](std::string const& key) const -> std::unique_ptr<dir_node> const&
    {
        return m_dirs.at(key);
    }

    [[nodiscard]] auto contains_dir(std::string const& key) const noexcept -> bool
    {
        return m_dirs.find(key) != m_dirs.end();
    }

    [[nodiscard]] auto data() noexcept -> T&
    {
        return m_data;
    }

    [[nodiscard]] auto data() const noexcept -> T const&
    {
        return m_data;
    }

    auto set_data(T&& data) noexcept -> void
    {
        m_data = std::forward<T>(data);
    }

    [[nodiscard]] auto get_dirs() noexcept -> dirs_t&
    {
        return m_dirs;
    }

    [[nodiscard]] auto get_dirs() const noexcept -> dirs_t const&
    {
        return m_dirs;
    }
};

using path_t = std::vector<std::string>;

auto to_path(std::string const& from) -> path_t;

class fs_tree
{
private:
    using ptr_t = dir_node<worker_state>*;
    using const_ptr_t = dir_node<worker_state> const*;

    static constexpr int s_indent_default = 4;

    dir_node<worker_state> m_tree{};
    mutable std::mutex m_mutex{};

    static auto print_indentation(std::ostream& os, int indent) -> void;
    auto pretty_print_to_impl(std::ostream& os, int indent, ptr_t ptr, std::string const& dir_name) -> void;

public:
    fs_tree() = default;
    fs_tree(fs_tree const&) = delete;
    fs_tree(fs_tree&&) = delete;
    ~fs_tree() noexcept = default;

    auto operator=(fs_tree const&) -> fs_tree& = delete;
    auto operator=(fs_tree &&) -> fs_tree& = delete;

    auto add(path_t const& path) -> void;
    [[nodiscard]] auto at(path_t const& path) const -> worker_state;
    auto set_at(path_t const& path, worker_state state) -> void;
    [[nodiscard]] auto is_already_being_processed(path_t const& path) const -> bool;
    [[nodiscard]] auto exists(path_t const& path) const noexcept -> bool;
    auto pretty_print_to(std::ostream& os, int indent = s_indent_default) -> void;
};

} // namespace da

#endif // !FILESYSTEM_HPP
