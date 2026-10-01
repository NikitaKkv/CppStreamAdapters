#pragma once

#include <deque>
#include <optional>
#include <unordered_map>

template <typename View, typename StartPos, typename Action, typename Key>
class AggregateByKeyView {
public:
    using incoming_type = typename View::value_type;
    using value_type = std::pair<std::invoke_result_t<Key, incoming_type>, StartPos>;

    AggregateByKeyView(AggregateByKeyView&& other) : 
        res_vec_(std::move(other.res_vec_)), 
        res_(std::move(other.res_)) {}

    AggregateByKeyView(View&& view, const StartPos& start, const Action& action, const Key& key) :
        res_vec_{},
        res_{}
        {
        while (auto v = view.Next()) {
            if (!res_.contains(key(v.value()))) {
                res_vec_.emplace_back(key(v.value()), start);
                res_.emplace(key(v.value()), start);
            }
            action(v.value(), res_[key(v.value())]);
        }
        for (auto& el : res_vec_) {
            el.second = res_[el.first];
        }
    }

    std::optional<value_type> Next() {
        while (!res_vec_.empty()) {
            auto cp = std::move(*res_vec_.begin());
            res_vec_.pop_front();
            return std::move(cp);
        }
        return std::nullopt;
    }

    struct Iterator {
        AggregateByKeyView* parent = nullptr; 

        Iterator& operator++() {
            parent->res_vec_.pop_front();
            if (parent->res_vec_.empty()) {
                *this = Iterator{};
            }
            return *this;
        }

        value_type operator*() {
            auto res = *parent->res_vec_.begin();
            return res;
        }

        bool operator!=(const Iterator& s) const {
            return parent != s.parent;
        };
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
    std::deque<value_type> res_vec_;
    std::unordered_map<std::invoke_result_t<Key, incoming_type>, StartPos> res_;
};

template<typename StartPos, typename Action, typename Key>
struct AggregateByKey {
    StartPos start;
    Action action;
    Key key;
};

template<typename ViewIn, typename StartPos, typename Action, typename Key>
auto operator|(ViewIn&& view, AggregateByKey<StartPos, Action, Key>&& adapter) {
    return AggregateByKeyView<ViewIn, StartPos, Action, Key>(
            std::move(view), std::move(adapter.start), std::move(adapter.action), std::move(adapter.key));
}