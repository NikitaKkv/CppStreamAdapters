#pragma once

#include <filesystem>
#include <optional>

using RecDirIt = std::filesystem::recursive_directory_iterator;
using DirIt = std::filesystem::directory_iterator;

template <typename It, typename Sent>
class DirView {
public:
    using value_type = std::filesystem::path;

    DirView(DirView&& other) : it_(std::move(other.it_)), sent_(std::move(other.sent_)) {}
    DirView(DirIt it, DirIt sent) : it_(it), sent_(sent) {}
    DirView(RecDirIt it, RecDirIt sent) : it_(it), sent_(sent) {}

    std::optional<value_type> Next() {
        if (it_ != sent_) {
            auto path = it_->path();
            ++it_;
            return path;
        }
        return std::nullopt;
    }

    struct Iterator {
        const DirView* parent; 

        Iterator& operator++() {
            ++(parent->it_);
            return *this;
        }

        value_type operator*() {
            if constexpr (std::is_copy_constructible_v<value_type>) {
                auto res = *parent->it_;
                return res;
            } else {
                auto res = std::move(*parent->it_);
                return res;
            }
        }

        bool operator!=(Sent s) const {
            return parent->it_ != s;
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

private:
    It it_;
    Sent sent_;
};

enum rec {
    // NOLINTNEXTLINE
    recursive
};

enum non_rec {
    // NOLINTNEXTLINE
    non_recursive
};



inline auto Dir(const std::filesystem::path& dir, rec) {
    RecDirIt it(dir);
    RecDirIt sent;
    return DirView<RecDirIt, RecDirIt>(it, sent);
}

inline auto Dir(const std::filesystem::path& dir, non_rec) {
    DirIt it(dir);
    DirIt sent;
    return DirView<DirIt, DirIt>(it, sent);
}