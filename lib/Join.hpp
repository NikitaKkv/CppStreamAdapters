#pragma once

#include <deque>
#include <optional>
#include <type_traits>
#include <vector>

template <typename Key, typename Value>
struct KV {
	Key key;
	Value value;
};

template <typename Base, typename Joined>
struct JoinResult {
	Base base;
	std::optional<Joined> joined;

    bool operator==(const JoinResult& other) const = default;
};

template<typename T>
// NOLINTNEXTLINE
struct is_KV : std::false_type {};

template<typename T, typename U>
struct is_KV<KV<T, U>> : std::true_type {};

template<typename T>
struct ParseKV {
    using Key = void;
    using Value = void;
};

template<typename K, typename V>
struct ParseKV<KV<K, V>> {
    using Key = K;
    using Value = V;
};

template <typename LeftView, typename RightView, typename KeyLeft, typename KeyRight>
class JoinView {
private:
    template<typename T>
    auto GetValue(const T& val) {
        if constexpr (is_KV<T>::value) {
            return val.value;
        }
        else {
            return val;
        }
    }

    template<typename T, typename Key>
    auto GetKey(const T& val, std::optional<Key> key) {
        if constexpr (is_KV<T>::value) {
            return val.key;
        }
        else {
            return key.value()(val);
        }
    }

public:
    using LeftV = typename LeftView::value_type;
    using RightV = typename RightView::value_type;
    using value_type = JoinResult<
                            std::conditional_t<
                                is_KV<LeftV>::value, 
                                typename ParseKV<LeftV>::Value, 
                                LeftV>,

                            std::conditional_t<
                                is_KV<RightV>::value, 
                                typename ParseKV<RightV>::Value, 
                                RightV>>;

    JoinView(JoinView&& other) : 
        res_(std::move(other.res_)) {}

    JoinView(LeftView&& left_view, RightView&& right_view, std::optional<KeyLeft>&& keyL, std::optional<KeyRight>&& keyR) :
        res_{}
        {
        std::vector<typename LeftView::value_type> left;
        std::vector<typename RightView::value_type> right;
        while (auto v = left_view.Next()) {
            left.emplace_back(std::move(*v));
        }
        while (auto v = right_view.Next()) {
            right.emplace_back(std::move(*v));
        }

        for (const auto& el : left) {
            bool at_least = false;
            for (const auto& right_el : right) {
                if (GetKey(el, keyL) == GetKey(right_el, keyR)) {
                    at_least = true;
                    res_.emplace_back(GetValue(el), GetValue(right_el));
                }
            }
            if (!at_least) {
                res_.emplace_back(GetValue(el), std::nullopt);
            }
        }
    }

    std::optional<value_type> Next() {
        while (!res_.empty()) {
            auto cp = std::move(*res_.begin());
            res_.pop_front();
            return std::move(cp);
        }
        return std::nullopt;
    }

    struct Iterator {
        JoinView* parent = nullptr; 

        Iterator& operator++() {
            parent->res_.pop_front();
            if (parent->res_.empty()) {
                *this = Iterator{};
            }
            return *this;
        }

        value_type operator*() {
            auto res = *parent->res_.begin();
            return res;
        }

        bool operator!=(const Iterator& s) const = default;
    };

    // NOLINTNEXTLINE
    Iterator begin() {
        return Iterator{this};
    }

    // NOLINTNEXTLINE
    Iterator end() {
        return Iterator{};
    }

private:
    std::deque<value_type> res_;
};

template<typename RightView, typename KeyLeft = int, typename KeyRight = int>
struct Join {
    Join(RightView&& view) :
        right_view(std::move(view)) {}
    Join(RightView& view) :
        right_view(std::move(view)) {}
    Join(RightView&& view, KeyLeft keyleft, KeyRight keyright) :
        right_view(std::move(view)), key_left(std::move(keyleft)), key_right(std::move(keyright)) {}
    Join(RightView& view, KeyLeft keyleft, KeyRight keyright) :
        right_view(std::move(view)), key_left(std::move(keyleft)), key_right(std::move(keyright)) {}

    RightView right_view;
    std::optional<KeyLeft> key_left;
    std::optional<KeyRight> key_right;
};

template<typename LeftView, typename RightView, typename KeyLeft, typename KeyRight>
auto operator|(LeftView&& left_view, Join<RightView, KeyLeft, KeyRight>&& adapter) {
    using PureLeftView = std::remove_reference_t<LeftView>;

    return JoinView<PureLeftView, RightView, KeyLeft, KeyRight>(
        std::move(left_view),         
        std::move(adapter.right_view), 
        std::move(adapter.key_left), 
        std::move(adapter.key_right)
    );
}