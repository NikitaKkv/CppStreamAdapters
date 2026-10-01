#pragma once

#include <expected>
#include <memory>
#include <optional>
#include <queue>
#include <type_traits>
#include <utility>

template <typename T>
// NOLINTNEXTLINE
struct expected_types {
    using value_type = T;
    using error_type = T;
};

template <typename T, typename U>
struct expected_types<std::expected<T, U>> {
    using value_type = T;
    using error_type = U;
};


template <typename T>
// NOLINTNEXTLINE
struct is_expected {
    // NOLINTNEXTLINE
    static const bool value = false;
};

template <typename T, typename U>
struct is_expected<std::expected<T, U>> {
    // NOLINTNEXTLINE
    static const bool value = true;
};


template <typename View>
class ExpectedView {
public:
    using value_type = typename View::value_type;

    ExpectedView(ExpectedView&& other) : view_(std::move(other.view_)) {}
    ExpectedView(std::shared_ptr<View> view) : view_(view) {}

    std::optional<value_type> Next() {
        return view_->NextValue();
    }
private:
    std::shared_ptr<View> view_;
};

template <typename View>
class UnExpectedView {
public:
    using value_type = typename View::error_type;

    UnExpectedView(UnExpectedView&& other) : view_(std::move(other.view_)) {}
    UnExpectedView(std::shared_ptr<View> view) : view_(view) {}

    std::optional<value_type> Next() {
        return view_->NextError();
    }

private:
    std::shared_ptr<View> view_;
};

template <typename View>
class SplitExpectedView {
public:
    using value_type = typename expected_types<typename View::value_type>::value_type;
    using error_type = typename expected_types<typename View::value_type>::error_type;

    SplitExpectedView (View&& view) : 
        view_(std::move(view)), 
        errors_{},
        values_{} {}

    SplitExpectedView (SplitExpectedView&& other) : 
        view_(std::move(other.view_)), 
        errors_(std::move(other.errors_)),
        values_(std::move(other.values_)) {}

    std::optional<value_type> NextValue() {
        if constexpr (is_expected<typename View::value_type>::value) {
            if (!values_.empty()) {
                value_type val = std::move(values_.front());
                values_.pop();
                return std::move(val);
            }
            while (auto opt = view_.Next()) {
                if (opt.value().has_value()) {
                    return std::move(opt.value().value());
                }
                else {
                    errors_.push(std::move(opt.value().error()));
                }
            }
            return std::nullopt;
        }
        else {
            return view_.Next();
        }
    }

    std::optional<error_type> NextError() {
        if constexpr (is_expected<typename View::value_type>::value) {
            if (!errors_.empty()) {
                error_type err = std::move(errors_.front());
                errors_.pop();
                return std::move(err);
            }
            while (auto opt = view_.Next()) {
                if (!opt.value().has_value()) {
                    return std::move(opt.value().error());
                }
                else {
                    values_.push(std::move(opt.value().value()));
                }
            }
            return std::nullopt;
        }
        else {
            return std::nullopt;
        }
    }

private:
    std::queue<value_type> values_;
    std::queue<error_type> errors_;
    View view_;
};

struct SplitExpected {};

template<typename ViewIn>
auto operator|(ViewIn&& view, SplitExpected adapter) {

    using CleanView = std::decay_t<ViewIn>;

    std::shared_ptr<SplitExpectedView<CleanView>> split_view = 
        std::make_shared<SplitExpectedView<CleanView>>(std::move(view));

    return std::make_pair(UnExpectedView<SplitExpectedView<CleanView>>{split_view}, 
                          ExpectedView<SplitExpectedView<CleanView>>{split_view});
}