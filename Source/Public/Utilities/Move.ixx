export module ntl.utils.move;

template<class A>
struct UnRefHelper { using Type = A; };
template<class A>
struct UnRefHelper<A&> { using Type = A; };
template<class A>
struct UnRefHelper<A&&> { using Type = A; };

export namespace ne {
    template<class T>
    [[nodiscard]]
    constexpr
    typename UnRefHelper<T>::Type&& Move(T&& t) noexcept {
        return static_cast<typename UnRefHelper<T>::Type&&>(t);
    }

}