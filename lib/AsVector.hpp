#pragma once

#include <type_traits>
#include <vector>

struct AsVector {};

template<typename ViewIn>
auto operator|(ViewIn&& view, AsVector out) {
    using value_type = typename std::remove_reference_t<ViewIn>::value_type;
    std::vector<value_type> vec;
    while (auto v = view.Next()) {
        vec.push_back(std::move(v.value()));
    }
    return vec;
}