#pragma once

#include <algorithm>
#include <iterator>
#include <optional>
#include <type_traits>

template <typename It, typename Sent>
class DataView {

    mutable It current_it_; 
    Sent sent_;

public:
    using value_type = std::iter_value_t<It>;

    DataView(DataView&& other) : current_it_(std::move(other.current_it_)), sent_(std::move(other.sent_)) {}
    DataView(It it, Sent sent) : current_it_(it), sent_(sent) {}

    std::optional<value_type> Next() {
        if (current_it_ != sent_) {
            if constexpr (std::is_copy_constructible_v<value_type>) {
                auto res = *current_it_;
                ++current_it_;
                return res;
            } else {
                auto res = std::move(*current_it_);
                ++current_it_;
                return res;
            }
        }
        return std::nullopt;
    }
    

    struct Iterator {
        const DataView* parent; 

        Iterator& operator++() {
            ++(parent->current_it_);
            return *this;
        }

        value_type operator*() {
            if constexpr (std::is_copy_constructible_v<value_type>) {
                auto res = *parent->current_it_;
                return res;
            } else {
                auto res = std::move(*parent->current_it_);
                return res;
            }
        }

        bool operator!=(Sent s) const {
            return parent->current_it_ != s;
        }
    };

    // NOLINTNEXTLINE
    Iterator begin() const {
        return Iterator{this};
    }

    // NOLINTNEXTLINE
    Sent end() const {
        return sent_;
    }
};

template <typename Container>
auto AsDataFlow(Container&& cont) {
    auto it = cont.begin();
    auto sent = cont.end();
    return DataView<decltype(it), decltype(sent)>(it, sent);
}