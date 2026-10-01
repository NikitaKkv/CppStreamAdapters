#pragma once

#include <ostream>

struct Out {
    std::ostream& os;
};

template<typename ViewIn>
std::ostream& operator|(ViewIn&& view, const Out& out) {
    while (auto v = view.Next()) {
        out.os << v.value();
    }
    return out.os;
}