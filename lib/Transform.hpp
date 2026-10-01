#include <optional>
#include <utility>

template <typename View, typename Transform>
class TransformView {
public:
    using incoming_type = typename View::value_type;
    
    using value_type = std::invoke_result_t<Transform, incoming_type>;

    TransformView(TransformView&& other) : 
        view_(std::move(other.view_)),
        transform_(std::move(other.transform_)) {}

    TransformView(View&& view, Transform&& transform) : 
        view_(std::move(view)),
        transform_(std::move(transform)) {}

    std::optional<value_type> Next() {
        if (auto v = view_.Next()) {
            return transform_(std::move(v.value()));
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
    Transform transform_;
    View view_;
};

template<typename T>
struct Transform {
    T transform;
};

template<typename ViewIn, typename Op>
auto operator|(ViewIn&& view, Transform<Op>&& op) {
    return TransformView<ViewIn, Op>(std::move(view), std::move(op.transform));
}