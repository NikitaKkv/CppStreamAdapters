#pragma once

#include <optional>

template <typename T>
// NOLINTNEXTLINE
struct optional_type {
    using type = T;
};

template <typename T>
struct optional_type<std::optional<T>> {
    using type = T;
};


template <typename T>
// NOLINTNEXTLINE
struct is_optional {
    // NOLINTNEXTLINE
    static const bool value = false;
};

template <typename T>
struct is_optional<std::optional<T>> {
    // NOLINTNEXTLINE
    static const bool value = true;
};


template <typename View>
class DropNulloptView {
public:
    using value_type = typename optional_type<typename View::value_type>::type;

    DropNulloptView(DropNulloptView&& other) : view_(std::move(other.view_)) {}
    DropNulloptView(View&& view) : view_(std::move(view)) {}

    std::optional<value_type> Next() {
        if constexpr (is_optional<typename View::value_type>::value) {
            while (auto opt = view_.Next()) {
                if (opt.value().has_value()) {
                    return std::move(opt.value()); 
                }
            }
            return std::nullopt;
        }
        else {
            return view_.Next(); 
        }
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
    View view_;
};

struct DropNullopt {};

template<typename ViewIn>
auto operator|(ViewIn&& view, DropNullopt adapter) {
    return DropNulloptView<ViewIn>(std::move(view));
}