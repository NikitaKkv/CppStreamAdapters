#pragma once

#include <fstream>
#include <optional>
#include <utility>

template <typename View>
class OpenFilesView {
public:
    using value_type = std::ifstream;

    OpenFilesView(OpenFilesView&& other) : 
        view_(std::move(other.view_)) {}
    
    OpenFilesView(View&& view) : 
        view_(std::move(view)) {}

    std::optional<value_type> Next() {
        if (auto path = view_.Next()) {
            std::ifstream flow(path.value());
            return std::move(flow);
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
    View view_;
};

struct OpenFiles {};

template<typename ViewIn>
auto operator|(ViewIn&& view, OpenFiles of) {
    return OpenFilesView<ViewIn>(std::move(view));
}