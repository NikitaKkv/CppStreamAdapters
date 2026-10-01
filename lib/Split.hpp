#pragma once

#include <algorithm>
#include <optional>
#include <string>
#include <utility>

enum ReadRes{
    kNewFlow,
    kOldFlow,
    kLackOfFlow
};

template <typename View>
class SplitView {
public:
    using value_type = std::string;

    SplitView(std::string&& dels, View&& view) : 
        buffer_(kBufferSize_, '\0'),   
        word_end_(-1),
        delims_(std::move(dels)),
        view_(std::move(view)) {}

    SplitView(SplitView&& other) : 
        buffer_(std::move(other.buffer_)),   
        word_end_(std::move(other.word_end_)),
        delims_(std::move(other.delims_)),
        view_(std::move(other.view_)) {}

    ReadRes ReadFlow() {

        while (true) {
            if (flow_ && !flow_.eof()) {
                flow_.read(buffer_.data(), kBufferSize_);
                if (flow_.gcount() == 0) {
                    continue;
                }
                return kOldFlow;
            }
            else if (auto v = view_.Next()) {
                flow_ = std::move(v.value());
                flow_.read(buffer_.data(), kBufferSize_);
                if (flow_.gcount() == 0) {
                    continue;
                }
                return kNewFlow;
            }
            else {
                lack_of_flow_ = true;
                return kLackOfFlow;
            }
        }
    }


    std::optional<value_type> Next() {
        std::string token = "";

        if (lack_of_flow_) {
            return std::nullopt;
        }

        if (word_end_ == -1) {
            ReadFlow();
            word_end_ = 0;
        }

        while (true) {
            if (word_end_ < flow_.gcount() && delims_.contains(buffer_[word_end_])) {
                ++word_end_;
            }
            
            while (word_end_ < flow_.gcount() && !delims_.contains(buffer_[word_end_])) {
                token += buffer_[word_end_];
                ++word_end_;
            }

            if (word_end_ == flow_.gcount()) {
                word_end_ = 0;
                if (ReadFlow() == kOldFlow && !delims_.contains(buffer_[0])) {
                    continue;
                }
                return std::move(token);
            }
            return std::move(token);
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

    const int kBufferSize_ = 10000;

    bool lack_of_flow_ = false;
    typename View::value_type flow_;
    int word_end_;
    std::string buffer_; 
    std::string delims_;
    View view_;
};

struct Split {
    std::string delims;
};

template<typename ViewIn>
auto operator|(ViewIn&& view, Split&& adapter) {
    return SplitView<ViewIn>(std::move(adapter.delims), std::move(view));
}