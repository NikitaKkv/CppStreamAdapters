#pragma once

#include <optional>
template <typename View, typename Cond>
class FilterView {
public:
    using value_type = typename View::value_type;

    FilterView(FilterView&& other) : 
        view_(std::move(other.view_)),
        cond_(std::move(other.cond_)) {}

    FilterView(View&& view_, Cond&& cond_) : 
        view_(std::move(view_)),
        cond_(std::move(cond_)) {}

    std::optional<value_type> Next() {
        while (auto opt = view_.Next()) {
            if (cond_(opt.value())) {
                return std::move(opt.value());
            }
        }
        return std::nullopt;
    }

    // NOLINTNEXTLINE
    auto begin() const {
        return view_.begin();
    }

    // NOLINTNEXTLINE
    auto end() const {
        return view_.end();
    }

private:
    Cond cond_;
    View view_;
};

template<typename T>
struct Filter {
    T cond_;
};

template<typename ViewIn, typename Cond>
auto operator|(ViewIn&& view, Filter<Cond>&& adapter) {
    return FilterView<ViewIn, Cond>(std::move(view), std::move(adapter.cond_));
}