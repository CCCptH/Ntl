export module ntl.type_traits.swappable_traits;
import ntl.utils.swap;
import ntl.utils.declval;

export namespace ne
{
    template<class T>
	struct WithRefSwappable
	{
        using Type = T&;
	};
}

export namespace ne
{
	template<class A, class B>
    inline constexpr bool TestIsSwappableWith = requires {
        utils::Swap(Declval<A>(), Declval<B>());
        utils::Swap(Declval<B>(), Declval<A>());
    };
    template<class T>
    inline constexpr bool TestIsSwappable = requires
    {
        typename WithRefSwappable<T>::Type;
    } and TestIsSwappableWith<T, T>;

    template<class A, class B>
    inline constexpr bool TestIsNothrowSwappableWith =
        TestIsSwappableWith<A, B> and
        noexcept(utils::Swap(Declval<A>(), Declval<B>())) and
        noexcept(utils::Swap(Declval<A>(), Declval<B>()));
    template<class T>
    inline constexpr bool TestIsNothrowSwappable =
        TestIsSwappable<T> and
        noexcept(utils::Swap(Declval<T>(), Declval<T>()));
}
