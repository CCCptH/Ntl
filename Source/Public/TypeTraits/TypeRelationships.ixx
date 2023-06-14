module;
#include <type_traits>
export module ntl.type_traits.type_relationships;
import ntl.type_traits.base_classes;
import ntl.type_traits.type_modifications;

namespace ne
{
    template<class A, class B>
    struct TestIsSameHelper { static constexpr bool VALUE = false;};
    template<class A>
    struct TestIsSameHelper<A, A> {static constexpr bool VALUE  = true;};


    template<class Base, class Derive>
    struct TestIsBaseOfHelper: BooleanConstant<std::is_base_of_v<Base, Derive>>{};

    template<class From, class To>
    struct TestIsConvertibleHelper: BooleanConstant<std::is_convertible_v<From, To>>{};

    template<class From, class To>
    struct TestIsNothrowConvertibleHelper: BooleanConstant<std::is_nothrow_convertible_v<From, To>>{};
}

export namespace ne
{
    namespace details {

    }
    /**
     * @brief Same as std::is_same
     * @tparam A
     * @tparam B
     */
    template<class A, class B>
    inline constexpr bool TestIsSame = TestIsSameHelper<A, B>::VALUE;
    /***
     * @brief Same as std::is_base_of
     * @tparam Base
     * @tparam Derive
     */
    template<class Base, class Derive>
    inline constexpr bool TestIsBaseOf = TestIsBaseOfHelper<Base, Derive>::VALUE;
    /**
     * @brief Same as std::is_convertible
     * @tparam From
     * @tparam To
     */
    template<class From, class To>
    inline constexpr bool TestIsConvertible = TestIsConvertibleHelper<From, To>::VALUE;
    /**
     * @brief Same as std::is_nothrow_convertible
     * @tparam From
     * @tparam To
     */
    template<class From, class To>
    inline constexpr bool TestIsNothrowConvertible = TestIsNothrowConvertibleHelper<From, To>::VALUE;
    // Todo:
    // 1. is_invocable
    // 2. is_invocable_r
    // 3. is_nothrow_invocable
    // 4. is_nothrow_invocable_r
}