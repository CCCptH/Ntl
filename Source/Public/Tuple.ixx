module;
#include <tuple>
export module ntl.tuple;

import ntl.type_traits;
import ntl.utils;
import ntl.memory.allocator;

namespace ne
{
	template<class T, class TupleType, size_t ...I>
	constexpr T ConstructFromTupleImpl(TupleType&& t, IndexSequence<I...>)
	{
		static_assert(TestIsConstructible<T, decltype(std::get<I>(Declval<TupleType>()))...>,
			"[ntl.tuple] Can not construct class from tuple."
			);

		return T(std::get<I>(Forward<TupleType>(t))...);
	}
	template<class T, class TupleType, size_t ...I>
	constexpr T* ConstructFromTupleAtImpl(T* p, TupleType&& t, IndexSequence<I...>)
	{
		static_assert(TestIsConstructible<T, decltype(std::get<I>(Declval<TupleType>()))...>,
			"[ntl.tuple] Can not construct class from tuple."
			);

		return Construct(p, std::get<I>(Forward<TupleType>(t))...);
	}
}

export namespace ne
{
	template<class ...Ts>
	using Tuple = std::tuple<Ts...>;
	template<class ...Ts>
	Tuple<Ts&&...> ForwardAsTuple(Ts&&...args)
	{
		return Tuple<Ts&&...>(Forward<Ts>(args)...);
	}

	template<class T, class TupleType>
	constexpr T ConstructFromTuple(TupleType&& t)
	{
		return ConstructFromTupleImpl<T>(Forward<TupleType>(t), MakeIndexSequence<std::tuple_size_v<TypeUnRef<TupleType>>>{});
	}
	template<class T, class TupleType>
	constexpr T* ConstructFromTupleAt(T* p, TupleType&& t)
	{
		return ConstructFromTupleAtImpl<T>(Forward<TupleType>(t), MakeIndexSequence<std::tuple_size_v<TypeUnRef<TupleType>>>{});
	}

	template<size_t I, class ...Ts>
	constexpr std::tuple_element_t<I, Tuple<Ts...>>& Get(Tuple<Ts...>& t)
	{
		return std::get<I>(t);
	}
	template<size_t I, class ...Ts>
	constexpr const std::tuple_element_t<I, Tuple<Ts...>>& Get(const Tuple<Ts...>& t)
	{
		return std::get<I>(t);
	}
	template<size_t I, class ...Ts>
	constexpr std::tuple_element_t<I, Tuple<Ts...>>&& Get(Tuple<Ts...>&& t)
	{
		// Fuck that Rref is lvalue.
		return std::get<I>(Move(t));
	}
	template<size_t I, class ...Ts>
	constexpr const std::tuple_element_t<I, Tuple<Ts...>>&& Get(const Tuple<Ts...>&& t)
	{
		return std::get<I>(Move(t));
	}
}

// Fuck that it works on llvm, shit
//export namespace ne
//{
//    /**
//     * @brief 递归实现 ，同 std::Tuple
//     * @tparam Types
//     */
//    template<class ...Elems>
//    class Tuple;
//
//    template<class Tuple>
//    struct TupleSize;
//
//    template<size_t I, class T>
//    struct TupleElement;
//
//    template<size_t I,  class TupleType>
//    using TypeTupleElem = typename TupleElement<I,TupleType>::Type;
//
//    template<size_t I, class TupleType>
//    using TypeConstTupleElem = TypeConditional<
//        TestIsLRef<TypeTupleElem<I, TupleType>>,
//        TypeAddLRef<const TypeUnRef<TypeTupleElem<I, TupleType>>>,
//        const TypeTupleElem<I, TupleType>
//    >;
//
//    template<size_t I, typename ...Types>
//    constexpr TypeTupleElem<I, Tuple<Types...>>& Get(Tuple<Types...>& tuple) noexcept;
//
//    template<size_t I, typename ...Types>
//    constexpr TypeConstTupleElem<I, Tuple<Types...>>& Get(const Tuple<Types...>& tuple) noexcept;
//
//    template<typename T, typename ...Types>
//    constexpr T& Get(Tuple<Types...>& tuple) noexcept;
//
//    template<typename T, typename ...Types>
//    constexpr const T& Get(const Tuple<Types...>& tuple) noexcept;
//
//    template<typename T, typename ...Types>
//    constexpr T&& Get(Tuple<Types...>&& tuple) noexcept;
//
//    namespace details
//    {
//        template<typename Tp>
//        struct TestIsTupleNonEmptyHelper : BooleanConstant<TestIsEmpty<Tp>> { };
//
//        template<typename El0, typename... El>
//        struct TestIsTupleNonEmptyHelper<Tuple <El0, El...>> : FalseConstant { };
//
//        // 对非final的空类使用空基类优化
//        template<typename Tp>
//        using TestIsEmptyNotFinalHelper
//            = TypeConditional<TestIsFinal<Tp>, FalseConstant, TestIsTupleNonEmptyHelper< Tp>>;
//
//        template<size_t Idx, class Head, bool IS_EMPTY_CLASS = TestIsEmptyNotFinalHelper<Head>::VALUE>
//        struct TupleHead;
//
//        template<size_t Idx, class Head>
//        struct TupleHead<Idx, Head, true>: public Head
//        {
//            constexpr TupleHead(): Head(){}
//            constexpr TupleHead(const Head& h): Head(h){}
//            constexpr TupleHead(const TupleHead&) = default;
//            constexpr TupleHead(TupleHead&& ) = default;
//
//            template<class U>
//            constexpr TupleHead(U&& h) : Head(Forward<U>(h))
//            {}
//
//            static constexpr Head& Get(TupleHead& t) {
//                return t;
//            }
//
//            static constexpr const Head& Get(const TupleHead& t) {
//                return t;
//            }
//        };
//
//        template<size_t Idx, class Head>
//        struct TupleHead<Idx, Head, false>
//        {
//            Head head;
//            constexpr TupleHead(): head()
//            {
//                static_assert(!TestIsRef<Head>, "[ntl.tuple] Reference can not be default constructed");
//            }
//            constexpr TupleHead(const Head& h): head(h){}
//            constexpr TupleHead(const TupleHead&) = default;
//            constexpr TupleHead(TupleHead&&) = default;
//
//            template<class U>
//            constexpr TupleHead(U&& h)
//                : head(Forward<U>(h))
//            {}
//
//            static constexpr Head& Get(TupleHead& t) {
//                return t.head;
//            }
//
//            static constexpr const Head& Get(const TupleHead& t) {
//                return t.head;
//            }
//        };
//
//        // 递归版本Tuple实现
//        template<size_t Idx, class...Elems>
//        struct TupleImpl;
//
//        // 递归实现
//        template<size_t Idx, class Head, class ... Tails>
//        struct TupleImpl<Idx, Head, Tails...>: public TupleImpl<Idx+1, Tails...>, private TupleHead<Idx, Head>
//        {
//            using Inherited = TupleImpl<Idx+1, Tails...>;
//            using Base = TupleHead<Idx, Head>;
//            constexpr Inherited& getTail() { return *this; }
//
//            constexpr static Head& GetHead(TupleImpl&  t) { return Base::Get(t); }
//            constexpr static const Head& GetHead(const TupleImpl&  t) { return Base::Get(t); }
//
//            constexpr static Inherited& GetTails(TupleImpl& t) { return t; }
//            constexpr static const Inherited& GetTails(const TupleImpl& t) { return t; }
//
//            constexpr TupleImpl(): Base(), Inherited() {}
//            constexpr explicit TupleImpl(const Head& h, const Tails&...tails)
//                : Base(h)
//                , Inherited(tails...)
//            {}
//
//            template<class UHead, class ...UTails>
//                requires (sizeof...(Tails)==sizeof...(UTails))
//            explicit constexpr TupleImpl(UHead&& h, UTails&&...ts)
//                : Base(Forward<UHead>(h))
//                , Inherited(Forward<UTails>(ts)...)
//            {}
//
//            constexpr TupleImpl(const TupleImpl&) = default;
//            constexpr TupleImpl(TupleImpl&&) noexcept(TestIsNothrowMoveConstructible<TupleImpl>) = default;
//
//            template<class ...UElems>
//            constexpr TupleImpl(const TupleImpl<Idx, UElems...>& in)
//                : Base(TupleImpl<Idx, UElems...>::GetHead(in))
//                , Inherited(TupleImpl<Idx, UElems...>::GetTails(in))
//            {}
//
//            template<class UHead, typename ...UTails>
//            constexpr TupleImpl(TupleImpl<Idx, UHead, UTails...>&& in)
//                : Inherited(Move(TupleImpl<Idx, UHead, UTails...>::GetTails(in)))
//                , Base(Forward<UHead>(TupleImpl<Idx, UHead, UTails...>::GetHead(in)))
//            {}
//
//            template<class ...UElems>
//            constexpr void assign(const TupleImpl<Idx, UElems...>& in)
//            {
//                GetHead(*this) = TupleImpl<Idx, UElems...>::GetHead(in);
//                GetTails(*this).assign(TupleImpl<Idx, UElems...>::GetTails(in));
//            }
//
//            template<class UHead, typename ...UTails>
//            constexpr void assign(TupleImpl<Idx, UHead, UTails...>&& in) {
//                GetHead(*this) = Forward<UHead>(TupleImpl<Idx, UHead, UTails...>::GetHead(in));
//                GetTails(*this).assign(Move(TupleImpl<Idx, UHead, UTails...>::GetTails(in)));
//            }
//
//            void swap(TupleImpl& in) {
//                Swap(GetHead(*this), GetHead(in));
//                Inherited::swap(GetTails(in));
//            }
//        };
//
//        // 终止递归条件
//        template<size_t Idx, class Head>
//        struct TupleImpl<Idx, Head>: private TupleHead<Idx, Head>
//        {
//            template<size_t, typename...>
//            friend class TupleImpl;
//
//            using Base = TupleHead<Idx, Head>;
//
//            constexpr static Head& GetHead(TupleImpl&  t) { return Base::Get(t); }
//            constexpr static const Head& GetHead(const TupleImpl&  t) { return Base::Get(t); }
//
//            constexpr TupleImpl():Base(){}
//            constexpr explicit TupleImpl(const Head& h) : Base(h){};
//            template<class U>
//            constexpr explicit TupleImpl(U&& h)
//                : Base(Forward<U>(h))
//            {}
//
//            constexpr TupleImpl(const TupleImpl&) = default;
//            constexpr TupleImpl(TupleImpl&&) noexcept(TestIsNothrowMoveConstructible<TupleImpl>) = default;
//
//            template<class UHead>
//            constexpr void assign(const TupleImpl<Idx, UHead>& in)
//            {
//                GetHead(*this) = TupleImpl<Idx, UHead>::GetHead(in);
//            }
//
//            template<class UHead>
//            constexpr void assign(TupleImpl<Idx, UHead>&& in) {
//                GetHead(*this) = Forward<UHead>(TupleImpl<Idx, UHead>::GetHead(in));
//            }
//
//            void swap(TupleImpl& in) {
//                Swap(GetHead(*this), GetHead(in));
//            }
//        };
//
//        template<size_t I, class Head, class ...Tails>
//        constexpr Head& GetImpl(TupleImpl<I, Head, Tails...>& tp)noexcept {
//            return TupleImpl<I, Head, Tails...>::GetHead(tp);
//        }
//
//        template<size_t I, class Head, class ...Tails>
//        constexpr const Head& GetImpl(const TupleImpl<I, Head, Tails...>& tp)  noexcept {
//            return TupleImpl<I, Head, Tails...>::GetHead(tp);
//        }
//
//        template<typename ...Ts>
//        struct TupleConstraints
//        {
//            template<class ...Us>
//            static constexpr bool IsImplicitlyConstructible(){
//                if (sizeof...(Ts)==sizeof ...(Us)) {
//                    return ValAnd<TestIsConstructible<Ts, Us>..., TestIsConvertible<Us, Ts>...>;
//                }
//                else {
//                    return false;
//                }
//            }
//            template<class ...Us>
//            static constexpr bool IsExplicitlyConstructible() {
//                if (sizeof...(Ts)==sizeof ...(Us)) {
//                    return ValAnd<TestIsConstructible<Ts, Us>..., ValNot<ValAnd<TestIsConvertible<Us, Ts>...>>>;
//                }
//                else {
//                    return false;
//                }
//            }
//
//            static constexpr bool IsImplicitlyDefaultConstructible() {
//                return ValAnd<TestIsImplicitlyConstructible<Ts>...>;
//            }
//            static constexpr bool IsExplicitlyDefaultConstructible() {
//                return ValAnd<TestIsDefaultConstructible<Ts>...,
//                        ValNot<
//                                ValAnd<TestIsImplicitlyConstructible<Ts>...>
//                            >
//                        >;
//            }
//        };
//    }
//
//    template<class ...Elems>
//    class Tuple: public details::TupleImpl<0, Elems...>
//    {
//    private:
//        using Inherited = details::TupleImpl<0, Elems...>;
//        using Constraints = details::TupleConstraints<Elems...>;
//
//        template<class...Tails>
//        constexpr static bool IsArgsValid() {
//            return sizeof...(Tails) == sizeof...(Elems);
//        }
//
//        template<class TheTuple, class = Tuple, class = TypeUnCVRef<TheTuple>>
//        struct TestUseOtherCtorHelper: FalseConstant {};
//        template<class TheTuple, class T, class U>
//        struct TestUseOtherCtorHelper<TheTuple, Tuple<T>, Tuple<U>>: TrueConstant {};
//        template<class TheTuple, class T>
//        struct TestUseOtherCtorHelper<TheTuple, Tuple<T>, Tuple<T>>: TrueConstant {};
//
//    public:
//        constexpr Tuple()
//            noexcept(ValAnd<TestIsNothrowDefaultConstructible<Elems>...>)
//            requires (Constraints::IsImplicitlyDefaultConstructible() == true)
//            : Inherited()
//        {}
//        constexpr explicit Tuple()
//            noexcept(ValAnd<TestIsNothrowDefaultConstructible<Elems>...>)
//            requires (Constraints::IsExplicitlyDefaultConstructible() == true)
//            : Inherited()
//        {}
//
//        constexpr Tuple(const Elems&... elems)
//            noexcept(ValAnd<TestIsNothrowCopyConstructible<const Elems&>...>)
//            requires (sizeof...(Elems)>=1) && (Constraints::template IsImplicitlyConstructible<Elems...>()==true)
//            : Inherited(elems...)
//        {}
//
//        constexpr explicit Tuple(const Elems&...elems)
//            noexcept(ValAnd<TestIsNothrowCopyConstructible<const Elems&>...>)
//            requires (sizeof...(Elems)>=1) && (Constraints::template IsExplicitlyConstructible<Elems...>()==true)
//            : Inherited(elems...)
//        {}
//
//        template<class ...UElems>
//            requires (IsArgsValid<UElems...>()==true) && (Constraints::template IsImplicitlyConstructible<UElems...>())
//        constexpr Tuple(UElems&&...ues)
//            noexcept(ValAnd<TestIsNothrowConstructible<Elems, UElems>...>)
//            : Inherited(Forward<UElems>(ues)...)
//        {}
//
//        template<class ...UElems>
//            requires (IsArgsValid<UElems...>()==true) && (Constraints::template IsExplicitlyConstructible<UElems...>())
//        constexpr explicit Tuple(UElems&&...ues)
//            noexcept(ValAnd<TestIsNothrowConstructible<Elems, UElems>...>)
//            : Inherited(Forward<UElems>(ues)...)
//        {}
//
//        constexpr Tuple(const Tuple&) = default;
//        constexpr Tuple(Tuple&&) = default;
//
//        template<class...UElems>
//            requires (sizeof...(UElems)==sizeof...(Elems)) && (!TestUseOtherCtorHelper<const Tuple<UElems...>&>::VALUE)
//            && (Constraints::template IsImplicitlyConstructible<const UElems&...>())
//        constexpr Tuple(const Tuple<UElems...>& in)
//            noexcept(ValAnd<TestIsNothrowConstructible<Elems, const UElems&>...>)
//            : Inherited(static_cast<const details::TupleImpl<0, UElems...>&>(in))
//        {}
//
//        template<class...UElems>
//        requires (sizeof...(UElems)==sizeof...(Elems)) && (!TestUseOtherCtorHelper<const Tuple<UElems...>&>::VALUE)
//                 && (Constraints::template IsExplicitlyConstructible<const UElems&...>())
//        constexpr explicit Tuple(const Tuple<UElems...>& in)
//        noexcept(ValAnd<TestIsNothrowConstructible<Elems, const UElems&>...>)
//                : Inherited(static_cast<const details::TupleImpl<0, UElems...>&>(in))
//        {}
//
//        template<class...UElems>
//        requires (sizeof...(UElems)==sizeof...(Elems)) && (!TestUseOtherCtorHelper<Tuple<UElems...>&&>::VALUE)
//                 && (Constraints::template IsImplicitlyConstructible<UElems...>())
//        constexpr Tuple(Tuple<UElems...>&& in)
//        noexcept(ValAnd<TestIsNothrowConstructible<Elems, UElems>...>)
//                : Inherited(static_cast<details::TupleImpl<0, UElems...>&&>(in))
//        {}
//
//        template<class...UElems>
//        requires (sizeof...(UElems)==sizeof...(Elems)) && (!TestUseOtherCtorHelper<Tuple<UElems...>&&>::VALUE)
//                 && (Constraints::template IsExplicitlyConstructible<UElems...>())
//        constexpr explicit Tuple(Tuple<UElems...>&& in)
//        noexcept(ValAnd<TestIsNothrowConstructible<Elems, UElems>...>)
//                : Inherited(static_cast<details::TupleImpl<0, UElems...>&&>(in))
//        {}
//
//        constexpr Tuple& operator=(const Tuple& in)
//            noexcept(ValAnd<TestIsNothrowAssignable<Elems, const Elems&>...>)
//            requires(ValAnd<TestIsAssignable<Elems, const Elems&>...>)
//        {
//            this->assign(in);
//            return *this;
//        }
//
//        constexpr Tuple& operator=(Tuple&& in)
//            noexcept(ValAnd<TestIsNothrowAssignable<Elems, Elems>...>)
//            requires(ValAnd<TestIsAssignable<Elems, Elems>...>)
//        {
//            this->assign(Move(in));
//            return *this;
//        }
//
//        template<class...UElems>
//            requires ValAnd<TestIsAssignable<Elems, const UElems&>...>
//        constexpr Tuple& operator=(const Tuple<UElems...>& in)
//            noexcept(ValAnd<TestIsNothrowAssignable<Elems, const UElems&>...>)
//        {
//            this->assign(in);
//            return *this;
//        }
//
//        template<class...UElems>
//            requires ValAnd<TestIsAssignable<Elems, UElems>...>
//        constexpr Tuple& operator=(Tuple<UElems...>&& in)
//            noexcept(ValAnd<TestIsNothrowAssignable<Elems, UElems>...>)
//        {
//            this->assign(Move(in));
//            return *this;
//        }
//
//        constexpr void swap(Tuple& in)
//        // Todo: noexcept swap noexcept
//        {
//            Inherited::swap(in);
//        }
//
//        template<size_t I>
//            requires (I<sizeof...(Elems))
//        TypeTupleElem<I, Tuple>& get() {
//            return details::GetImpl<I>(*this);
//        }
//
//        template<size_t I>
//        requires (I<sizeof...(Elems))
//        const TypeTupleElem<I, Tuple>& get() const {
//            return details::GetImpl<I>(*this);
//        }
//    };
//
//    template<>
//    class Tuple<>
//    {
//    public:
//        Tuple() = default;
//        void swap(Tuple&) noexcept {}
//
//    };
//
//    // 推导指引
//    template<class...Types>
//    Tuple(Types...) -> Tuple<Types...>;
//
//    template<class Tp>
//    struct TupleSize<const Tp>: TupleSize<Tp>{};
//
//    template<class Tp>
//    struct TupleSize<volatile Tp>: TupleSize<Tp>{};
//
//    template<class Tp>
//    struct TupleSize<const volatile Tp>: TupleSize<Tp>{};
//
//    template<class...Elems>
//    struct TupleSize<Tuple<Elems...>>: IntegralConstant<size_t, sizeof ...(Elems)>{};
//
//    template<class TheTuple>
//    inline constexpr size_t ValTupleSize = TupleSize<TheTuple>::VALUE;
//
//    template<size_t I, class Tp>
//    struct TupleElement<I, const Tp>
//    {
//        using Type = typename TypeAddConst<TupleElement<I, Tp>>::Type;
//    };
//
//    template<size_t I, class Tp>
//    struct TupleElement<I, volatile Tp>
//    {
//        using Type = typename TypeAddVolatile<TupleElement<I, Tp>>::Type;
//    };
//
//    template<size_t I, class Tp>
//    struct TupleElement<I, const volatile Tp>
//    {
//        using Type = typename TypeAddCV<TupleElement<I, Tp>>::Type;
//    };
//
//    template<size_t I, class Head, class ...Tails>
//    struct TupleElement<I, Tuple<Head, Tails...>>:
//            TupleElement<I-1, Tuple<Tails...>>
//    {};
//
//    template<class Head, class...Tails>
//    struct TupleElement<0, Tuple<Head, Tails...>>
//    {
//        using Type = Head;
//    };
//
//    template<size_t I>
//    struct TupleElement<I, Tuple<>>
//    {
//        static_assert(I<1, "Index I exceeds the size of tuple");
//    };
//
//    namespace details
//    {
//        template<class T, class ...Ts>
//        constexpr size_t FindTypeInPack() {
//            constexpr auto size = sizeof...(Ts);
//            constexpr bool flags[size] = {TestIsSame<T, Ts>...};
//            size_t n = size;
//            for (size_t i=0; i<size;i++) {
//                if(flags[i]) {
//                    if (n<size) {
//                        return size;
//                    }
//                    n=i;
//                }
//            }
//            return n;
//        }
//    }
//    template<size_t I, class ...Elems>
//        requires (I<sizeof...(Elems))
//    constexpr TypeTupleElem<I, Tuple<Elems...>>& Get(Tuple<Elems...>& tuple) noexcept {
//        return details::GetImpl<I>(tuple);
//    }
//
//    template<size_t I, class ...Elems>
//        requires (I<sizeof...(Elems))
//    constexpr const TypeTupleElem<I, Tuple<Elems...>>& Get(const Tuple<Elems...>& tuple) noexcept {
//        return details::GetImpl<I>(tuple);
//    }
//
//    template<size_t I, class ...Elems>
//        requires (I<sizeof...(Elems))
//    constexpr TypeTupleElem<I, Tuple<Elems...>>&& Get(Tuple<Elems...>&& tuple) noexcept {
//        using T = TypeTupleElem<I, Tuple<Elems...>>;
//        return Forward<T>(details::GetImpl<I>(tuple));
//    }
//
//    template<size_t I, class ...Elems>
//        requires (I<sizeof...(Elems))
//    constexpr const TypeTupleElem<I, Tuple<Elems...>>&& Get(const Tuple<Elems...>&& tuple) noexcept {
//        using T = TypeTupleElem<I, Tuple<Elems...>>;
//        return Forward<const T>(details::GetImpl<I>(tuple));
//    }
//
//    template<class T, class...Elems>
//    constexpr T& Get(Tuple<Elems...>& tp) noexcept {
//        constexpr auto sz = details::FindTypeInPack<T, Elems...>();
//        static_assert(sz != sizeof...(Elems), "[ntl.tuple] T is expected to be unique");
//        return details::GetImpl<sz>(tp);
//    }
//
//    template<class T, class...Elems>
//    constexpr const T& Get(const Tuple<Elems...>& tp) noexcept {
//        constexpr auto sz = details::FindTypeInPack<T, Elems...>();
//        static_assert(sz != sizeof...(Elems), "[ntl.tuple] T is expected to be unique");
//        return details::GetImpl<sz>(tp);
//    }
//
//    template<class T, class...Elems>
//    constexpr T&& Get(Tuple<Elems...>&& tp) noexcept {
//        constexpr auto sz = details::FindTypeInPack<T, Elems...>();
//        static_assert(sz != sizeof...(Elems), "[ntl.tuple] T is expected to be unique");
//        return Forward<T>(details::GetImpl<sz>(tp));
//    }
//
//    template<class T, class...Elems>
//    constexpr const T&& Get(const Tuple<Elems...>&& tp) noexcept {
//        constexpr auto sz = details::FindTypeInPack<T, Elems...>();
//        static_assert(sz != sizeof...(Elems), "[ntl.tuple] T is expected to be unique");
//        return Forward<const T>(details::GetImpl<sz>(tp));
//    }
//
//    template<class...Elems>
//    constexpr Tuple<Elems...> MakeTuple(Elems&& ...elems) {
//        using ResultType = Tuple<Elems...>;
//        return ResultType(Forward<Elems>(elems)...);
//    }
//
//    /**
//     * Todo: 1. MakeTuple
//     *       2. ForwardAsTuple
//     *       3. Tie
//     */
//
//    template<class ...Args>
//    Tuple<Args&&...> ForwardAsTuple(Args&&...args)
//    {
//        return Tuple<Args&&>(Forward<Args>(args)...);
//    }
//}
//
//export namespace std
//{
//    // 适配结构化绑定
//    // https://zh.cppreference.com/w/cpp/language/structured_binding
//    // 情况 2：绑定元组式类型
//    // 表达式 std::tuple_size<E>::value 必须是良构的整数常量表达式，且标识符的数量必须等于 std::tuple_size<E>::value。
//    template<class MyTuple>
//    struct tuple_size;
//    template<class...Elems>
//    struct tuple_size<ne::Tuple<Elems...>>{
//        static constexpr size_t value = ne::TupleSize<ne::Tuple<Elems...>>::VALUE;
//    };
//
//    template<size_t I, class T>
//    struct tuple_element;
//
//    template<size_t I, class ...Ts>
//    struct tuple_element<I, ne::Tuple<Ts...>>
//    {
//        using type = typename ne::TupleElement<I, ne::Tuple<Ts...>>::Type;
//    };
//
//    template<size_t I, class ...Elems>
//    constexpr typename tuple_element<I, ne::Tuple<Elems...>>::type& get(ne::Tuple<Elems...>& tp) noexcept {
//        return ne::Get<I>(tp);
//    }
//
//    template<size_t I, class ...Elems>
//    constexpr const typename tuple_element<I, ne::Tuple<Elems...>>::type & get(const ne::Tuple<Elems...>& tp) noexcept {
//        return ne::Get<I>(tp);
//    }
//
//    template<size_t I, class ...Elems>
//    constexpr typename tuple_element<I, ne::Tuple<Elems...>>::type && get(ne::Tuple<Elems...>&& tp) noexcept {
//        using T = typename tuple_element<I, ne::Tuple<Elems...>>::type ;
//        return ne::Forward<T>(ne::Get<I>(tp));
//    }
//
//    template<size_t I, class ...Elems>
//    constexpr const typename tuple_element<I, ne::Tuple<Elems...>>::type && get(const ne::Tuple<Elems...>&& tp) noexcept {
//        using T = typename tuple_element<I, ne::Tuple<Elems...>>::type ;
//        return ne::Forward<const T>(ne::Get<I>(tp));
//    }
//
//    template<class T, class...Elems>
//    constexpr T& get(ne::Tuple<Elems...>& tp) noexcept {
//        return ne::Get<T>(tp);
//    }
//
//    template<class T, class...Elems>
//    constexpr const T& get(const ne::Tuple<Elems...>& tp) noexcept {
//        return ne::Get<T>(tp);
//    }
//
//    template<class T, class...Elems>
//    constexpr T&& get(ne::Tuple<Elems...>&& tp) noexcept {
//        return ne::Forward<T>(ne::Get<T>(tp));
//    }
//
//    template<class T, class...Elems>
//    constexpr const T&& get(const ne::Tuple<Elems...>&& tp) noexcept {
//        return ne::Forward<const T>(ne::Get<T>(tp));
//    }
//}