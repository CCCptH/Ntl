module;
#include <type_traits>
export module ntl.type_traits.support_operations;

import ntl.utils.declval;
import ntl.type_traits.base_classes;
import ntl.type_traits.type_modifications;

namespace ne
{
    template<class A, class...Args>
    struct TestIsConstructibleHelper:
            BooleanConstant<
        std::is_constructible_v<A, Args...>
    >{};

    template<class A, class ...Args>
    struct TestIsTriviallyConstructibleHelper:
            BooleanConstant<
        std::is_trivially_constructible_v<A, Args...>
    >{};

    template<class A, class ...Args>
    struct TestIsNothrowConstructibleHelper: BooleanConstant<
            //TestIsConstructibleHelper<A, Args...>::VALUE &&
            //noexcept(A(Declval<Args>()...))
        std::is_nothrow_constructible_v<A, Args...>
    >{};

    // From eastl:
    // Note:
    // This type trait has a misleading and counter-intuitive name. It does not indicate whether an instance
    // of U can be assigned to an instance of T (e.g. t = u). Instead it indicates whether the assignment can be
    // done after adding rvalue references to both, as in add_rvalue_reference<T>::type = add_rvalue_reference<U>::type.
    // A counterintuitive result of this is that is_assignable<int, int>::value == false. The is_copy_assignable
    // trait indicates if a type can be assigned to its own type, though there isn't a standard C++ way to tell
    // if an arbitrary type is assignable to another type.
    // http://stackoverflow.com/questions/19920213/why-is-stdis-assignable-counter-intuitive
    template<class A, class B>
    struct TestIsAssignableHelper:
        BooleanConstant<
            std::is_assignable_v<A, B>
        >
    {};

    template<class A, class B>
    struct TestIsTriviallyAssignableHelper:
        BooleanConstant<
            std::is_trivially_assignable_v<A, B>
        >
    {};

    template<class A, class B>
    struct TestIsNothrowAssignableHelper:
        BooleanConstant<
            std::is_nothrow_assignable_v<A, B>
        >
    {};

    template<class A>
    struct TestIsDestructibleHelper:
        BooleanConstant<
            std::is_destructible_v<A>
    >{};

    template<class A>
    struct TestIsTriviallyDestructibleHelper:
            BooleanConstant<
        std::is_trivially_destructible_v<A>
    >{};

    template<class A>
    struct TestIsNothrowDestructibleHelper:
            BooleanConstant<
        std::is_nothrow_destructible_v<A>
    >{};

    template<class A>
    struct TestHasVirtualDestructorHelper:
        BooleanConstant<
        std::has_virtual_destructor_v<A>
    >{};

    template<class T>
    constexpr void ImplicitlyConstructibleHelper(const T&);
    template<class T>
    constexpr auto ImplicitlyConstructibleTest(const T&, decltype(ImplicitlyConstructibleHelper<const T&>({}))*  =  0) -> TrueConstant ;
    template<class>
    constexpr auto ImplicitlyConstructibleTest(...)->FalseConstant ;

    template<class T>
    struct TestIsImplicitlyConstructibleHelper: decltype(ImplicitlyConstructibleTest(Declval<T>())) {};
}

export namespace ne {

    template<class A, class ...Args>
    inline constexpr bool TestIsConstructible = TestIsConstructibleHelper<A, Args...>::VALUE;
    template<class A, class ...Args>
    inline constexpr bool TestIsTriviallyConstructible = TestIsTriviallyConstructibleHelper<A, Args...>::VALUE;
    template<class A, class ...Args>
    inline constexpr bool TestIsNothrowConstructible = TestIsNothrowConstructibleHelper<A, Args...>::VALUE;
    template<class A>
    inline constexpr bool TestIsDefaultConstructible = TestIsConstructibleHelper<A>::VALUE;
    template<class A>
    inline constexpr bool TestIsTriviallyDefaultConstructible = TestIsTriviallyConstructibleHelper<A>::VALUE;
    template<class A>
    inline constexpr bool TestIsNothrowDefaultConstructible = TestIsNothrowConstructibleHelper<A>::VALUE;
    template<class A>
    inline constexpr bool TestIsCopyConstructible = TestIsConstructible<A, TypeAddLRef<TypeAddConst<A>>>;
    template<class A>
    inline constexpr bool TestIsTriviallyCopyConstructible = TestIsTriviallyConstructible<A, TypeAddLRef<TypeAddConst<A>>>;
    template<class A>
    inline constexpr bool TestIsNothrowCopyConstructible = TestIsNothrowConstructible<A, TypeAddLRef<TypeAddConst<A>>>;
    template<class A>
    inline constexpr bool TestIsMoveConstructible = TestIsConstructible<A, TypeAddRRef<A>>;
    template<class A>
    inline constexpr bool TestIsTriviallyMoveConstructible = TestIsTriviallyConstructible<A, TypeAddRRef<A>>;
    template<class A>
    inline constexpr bool TestIsNothrowMoveConstructible = TestIsNothrowConstructible<A, TypeAddRRef<A>>;
    template<class A, class B>
    inline constexpr bool TestIsAssignable = TestIsAssignableHelper<A, B>::VALUE;
    template<class A, class B>
    inline constexpr bool TestIsTriviallyAssignable = TestIsTriviallyAssignableHelper<A, B>::VALUE;
    template<class A, class B>
    inline constexpr bool TestIsNothrowAssignable = TestIsNothrowAssignableHelper<A, B>::VALUE;
    template<class A>
    inline constexpr bool TestIsCopyAssignable= TestIsAssignable<TypeAddLRef<A>, TypeAddLRef<const A>>;
    template<class A>
    inline constexpr bool TestIsTriviallyCopyAssignable= TestIsTriviallyAssignable<TypeAddLRef<A>, TypeAddLRef<const A>>;
    template<class A>
    inline constexpr bool TestIsNothrowCopyAssignable = TestIsNothrowAssignable<TypeAddLRef<A>, TypeAddLRef<const A>>;
    template<class A>
    inline constexpr bool TestIsMoveAssignable= TestIsAssignable<TypeAddLRef<A>, TypeAddRRef<A>>;
    template<class A>
    inline constexpr bool TestIsTriviallyMoveAssignable= TestIsTriviallyAssignable<TypeAddLRef<A>, TypeAddRRef<A>>;
    template<class A>
    inline constexpr bool TestIsNothrowMoveAssignable = TestIsNothrowAssignable<TypeAddLRef<A>, TypeAddRRef<A>>;
    template<class A>
    inline constexpr bool TestIsDestructible = TestIsDestructibleHelper<A>::VALUE;
    template<class A>
    inline constexpr bool TestIsTriviallyDestructible = TestIsTriviallyDestructibleHelper<A>::VALUE;
    template<class A>
    inline constexpr bool TestIsNothrowDestructible = TestIsNothrowDestructibleHelper<A>::VALUE;
    template<class A>
    inline constexpr bool TestHasVirtualDestructor = TestHasVirtualDestructorHelper<A>::VALUE;
    template<class T>
    //inline  constexpr bool TestIsImplicitlyConstructible = TestIsImplicitlyConstructibleHelper<T>::VALUE && TestIsConstructible<T>;
    inline constexpr bool TestIsImplicitlyConstructible = requires
    {
        ImplicitlyConstructibleHelper<T>({});
    };
    // TODO:
    // 1. swappable...
}