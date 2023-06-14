export module ntl.type_traits.composite_type_categories;

import ntl.type_traits.base_classes;
import ntl.type_traits.type_modifications;
import ntl.type_traits.type_relationships;
import ntl.type_traits.primary_type_categories;

namespace ne {
    template <class A> struct TestIsNoCVMemberPointerHelper : FalseConstant {};
    template <class A, class U>
    struct TestIsNoCVMemberPointerHelper<A U::*> : TrueConstant {};
    template <class A>
    struct TestIsMemberPointerHelper : TestIsNoCVMemberPointerHelper<TypeUnCV<A>> {
    };

    template <class A> struct TestIsNoCVMemberFuncPtrHelper : FalseConstant {};
    template <class A, class U>
    struct TestIsNoCVMemberFuncPtrHelper<A U::*> : BooleanConstant<TestIsFunction<A>> {};
    template <class A>
    struct TestIsMemberFuncPtrHelper : TestIsNoCVMemberFuncPtrHelper<TypeUnCV<A>> {
    };

    template <class A>
    struct TestIsMemberObjPtrHelper
        : BooleanConstant<TestIsMemberPointerHelper<A>::VALUE &&
                        (!TestIsMemberFuncPtrHelper<A>::VALUE)> {};

    template <class A>
    struct TestIsArithmeticHelper
        : BooleanConstant<((TestIsIntegral<A>) || (TestIsFloatingPoint<A>))> {};

    template <class A>
    struct TestIsFundamentalHelper
        : BooleanConstant<TestIsArithmeticHelper<A>::VALUE || TestIsVoid<A> ||
                        TestIsNullptr<TypeUnCV<A>>> {};

    template <class A>
    struct TestIsScalarHelper
        : BooleanConstant<TestIsArithmeticHelper<A>::VALUE || TestIsEnum<A> ||
                        TestIsPointer<A> || TestIsNullptr<A> ||
                        TestIsMemberPointerHelper<A>::VALUE> {};

    template <class A>
    struct TestIsObjectHelper
        : BooleanConstant<TestIsScalarHelper<A>::VALUE || TestIsArray<A> ||
                        TestIsUnion<A> || TestIsClass<A>> {};

    template <class A>
    struct TestIsCompoundHelper
        : BooleanConstant<!(TestIsFundamentalHelper<A>::VALUE)> {};

    template <class A> struct TestIsRefHelper : FalseConstant {};
    template <class A> struct TestIsRefHelper<A &> : TrueConstant {};
    template <class A> struct TestIsRefHelper<A &&> : TrueConstant {};
} // namespace ne

export namespace ne
{

    /**
     * @brief Same as std::is_member_pointer
     * @tparam A
     */
    template<class A>
    inline constexpr bool TestIsMemberPointer = TestIsMemberPointerHelper<A>::VALUE;
    /**
     * @brief Same as std::is_member_function_pointer
     * @tparam A
     */
    template<class A>
    inline constexpr bool TestIsMemberFuncPtr = TestIsMemberFuncPtrHelper<A>::VALUE;
    /**
     * @brief Same as std::is_member_object_pointer
     * @tparam A
     */
    template<class A>
    inline constexpr bool TestIsMemberObjPtr = TestIsMemberObjPtrHelper<A>{};
    /**
     * @brief Same as std::is_arithmetic
     * @tparam A
     */
    template<class A>
    inline constexpr bool TestIsArithmetic = TestIsArithmeticHelper<A>::VALUE;
    /**
     * @brief Same as std::is_fundamental
     * @tparam A
     */
    template<class A>
    inline constexpr bool TestIsFundamental = TestIsFundamentalHelper<A>::VALUE;
    /**
     * @brief Same as std::is_scalar
     * @tparam A
     */
    template<class A>
    inline constexpr bool TestIsScalar = TestIsScalarHelper<A>::VALUE;
    /**
     * @brief Same as std::is_object
     * @tparam A
     */
    template<class A>
    inline constexpr bool TestIsObject = TestIsObjectHelper<A>::VALUE;
    /**
     * @brief Same as std::is_compound
     * @tparam A
     */
    template<class A>
    inline constexpr bool TestIsCompound = TestIsCompoundHelper<A>::VALUE;
    /**
     * @brief Same as std::is_reference
     * @tparam A
     */
    template<class A>
    inline constexpr bool TestIsRef = TestIsRefHelper<A>::VALUE;
    
    template<class A>
    inline constexpr bool TestIsReference = TestIsRef<A>;
}