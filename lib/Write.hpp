#pragma once

#include <ostream>

struct Write {
    Write(std::ostream& os, char delim) : os(os), delim(delim) {}
    std::ostream& os;
    char delim;
};

template<typename ViewIn>
bool operator|(ViewIn&& view, const Write& out) {
    while (auto v = view.Next()) {
        out.os << v.value() << out.delim;
    }
    return true;
}