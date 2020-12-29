#ifndef UNORDERED_MAP_HPP
#define UNORDERED_MAP_HPP
#pragma once

#include <mutex>
#include <unordered_map>

namespace da {

template<typename Key, typename Val>
struct unordered_map
{
private:
    std::unordered_map<Key, Val> m_data{};
    mutable std::mutex m_mutex{};

public:
    unordered_map() = default;
    unordered_map(unordered_map const&) = default;
    unordered_map(unordered_map&&) noexcept = default;
    ~unordered_map() noexcept = default;

    auto operator=(unordered_map const&) -> unordered_map& = default;
    auto operator=(unordered_map&&) noexcept -> unordered_map& = default;

    auto operator[](Key const& key) const -> Val
    {
        std::scoped_lock<std::mutex> lock{ m_mutex };
        return m_data.at(key);
    }

    auto insert_or_update(Key const& key, Val&& val) -> void
    {
        std::scoped_lock<std::mutex> lock{ m_mutex };
        m_data[key] = std::forward<Val>(val);
    }

    auto remove(Key const& key) -> void
    {
        std::scoped_lock<std::mutex> lock{ m_mutex };

        if(auto const it = m_data.find(key); it != m_data.end()) {
            m_data.erase(it);
        }
    }

    [[nodiscard]] auto contains(Key const& key) const noexcept -> bool
    {
        std::scoped_lock<std::mutex> lock{ m_mutex };
        return m_data.find(key) != m_data.end();
    }

    [[nodiscard]] auto empty() const noexcept -> bool
    {
        std::scoped_lock<std::mutex> lock{ m_mutex };
        return m_data.empty();
    }

    [[nodiscard]] auto size() const noexcept -> std::size_t
    {
        std::scoped_lock<std::mutex> lock{ m_mutex };
        return m_data.size();
    }

    ///
    /// \warning NOT thread-safe
    ///
    auto begin()
    {
        return m_data.begin();
    }

    ///
    /// \warning NOT thread-safe
    ///
    auto end()
    {
        return m_data.end();
    }
};

} // namespace da

#endif // !UNORDERED_MAP_HPP
