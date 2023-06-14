export module ntl.utils.forward;

import ntl.type_traits.primary_type_categories;
import ntl.type_traits.type_modifications;

export namespace ne {
    template<class T>
    [[nodiscard]]
    constexpr
    T&& Forward(TypeUnRef<T>& t) noexcept {
        return static_cast<T&&>(t);
    }

    template<class T>
    [[nodiscard]]
    constexpr
    T&& Forward(TypeUnRef<T>&& t)  noexcept {
        static_assert(!TestIsLRef<T>, "[ntl.utils.forward] Cannot forward a rvalue as a lvalue");
        return static_cast<T&&>(t);
    }
}
