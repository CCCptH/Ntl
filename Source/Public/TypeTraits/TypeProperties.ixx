module;
#include <type_traits>
export module ntl.type_traits.type_properties;
import ntl.type_traits.base_classes;
import ntl.type_traits.primary_type_categories;
import ntl.utils.declval;
namespace ne
{
    template<class T>
    struct TestIsConstHelper : FalseConstant{};
    template<class T>
    struct TestIsConstHelper<T const> : TrueConstant{};

    template<class T>
    struct TestIsVolatileHelper: FalseConstant{};
    template<class T>
    struct TestIsVolatileHelper<T volatile>: TrueConstant{};

    template<class T>
    struct TestIsTriviallyCopyableHelper:
            BooleanConstant<
                    std::is_trivially_copyable_v<T>
            >
    {};

    template<class T>
    struct TestIsTrivialHelper:
            BooleanConstant<
                    std::is_trivial_v<T>
            >{};

    template<class T>
    struct TestIsStandardLayoutHelper:
            BooleanConstant<
        std::is_standard_layout_v<T>
    >{};

    template<class A>
    struct TestHasUniqueObjRepresentationsHelper:
            BooleanConstant<
        std::has_unique_object_representations_v<A>
    >{};

    template<class A>
    struct TestIsPolymorphicHelper:
            BooleanConstant<
        std::is_polymorphic_v<A>
    >{};

    template<class A>
    struct TestIsEmptyHelper:
            BooleanConstant<
        std::is_empty_v<A>
    >{};

    template<class A>
    struct TestIsAbstractHelper:
            BooleanConstant<
        std::is_abstract_v<A>
    >{};

    template<class A>
    struct TestIsAggregateHelper:
            BooleanConstant<
        std::is_aggregate_v<A>
    >{};

    template<class A>
    struct TestIsSignedHelper:
            BooleanConstant<
        std::is_signed_v<A>
    >{};

    template<class A>
    struct TestIsUnsignedHelper:
            BooleanConstant<
        std::is_unsigned_v<A>
    >{};

    template<class A>
    struct TestIsBoundedArrayHelper: FalseConstant{};
    template<class A, size_t N>
    struct TestIsBoundedArrayHelper<A[N]>: TrueConstant{};

    template<class A>
    struct TestIsUnBoundedArrayHelper: FalseConstant{};
    template<class A>
    struct TestIsUnBoundedArrayHelper<A[]>: TrueConstant{};

    template<class A>
    struct TestIsFinalHelper: BooleanConstant<
            std::is_final_v<A>
            >{};

    namespace {
        template<class A>
        auto EnumSizable(int) -> decltype(sizeof(A), TrueConstant{});
        template<class>
        auto EnumSizeable(...)->FalseConstant;
        template<class A>
        auto EnumToInt(int) -> decltype(static_cast<FalseConstant(*)(int)>(nullptr)(Declval<A>()));
        template<class A>
        auto EnumToInt(...)-> TrueConstant;
        template<class A>
        inline constexpr bool TestIsScopedEnumImplHelper = BooleanConstant<
                decltype(EnumSizable<A>(0))::VALUE && decltype(EnumToInt<A>(0))::VALUE
        >::VALUE;
    }

    template<class A>
    struct TestIsScopedEnumHelper: FalseConstant{};
    template<class A>
    requires TestIsEnum<A>
    struct TestIsScopedEnumHelper<A>: BooleanConstant<TestIsScopedEnumImplHelper<A>>{};
}

export namespace ne {

    template<class A>
    inline constexpr bool TestIsConst = TestIsConstHelper<A>::VALUE;

    template<class A>
    inline constexpr bool TestIsVolatile = TestIsVolatileHelper<A>::VALUE;

    template<class A>
    inline constexpr bool TestIsTriviallyCopyable = TestIsTriviallyCopyableHelper<A>::VALUE;

    template<class A>
    inline constexpr bool TestIsTrivial = TestIsTrivialHelper<A>::VALUE;

    template<class A>
    inline constexpr bool TestIsStandardLayout = TestIsStandardLayoutHelper<A>::VALUE;

    template<class A>
    inline constexpr bool TestHasUniqueObjRepresentations = TestHasUniqueObjRepresentationsHelper<A>::VALUE;

    template<class A>
    inline constexpr bool TestIsPolymorphic = TestIsPolymorphicHelper<A>::VALUE;

    template<class A>
    inline constexpr bool TestIsEmpty = TestIsEmptyHelper<A>::VALUE;

    template<class A>
    inline constexpr bool TestIsAggregate = TestIsAggregateHelper<A>::VALUE;

    template<class A>
    inline constexpr bool TestIsAbstract = TestIsAggregateHelper<A>::VALUE;

    template<class A>
    inline constexpr bool TestIsSigned = TestIsSignedHelper<A>::VALUE;

    template<class A>
    inline constexpr bool TestIsUnsigned = TestIsUnsignedHelper<A>::VALUE;

    template<class A>
    inline constexpr bool TestIsBoundedArray = TestIsBoundedArrayHelper<A>::VALUE;

    template<class A>
    inline constexpr bool TestIsUnboundedArray = TestIsUnBoundedArrayHelper<A>::VALUE;

    template<class A>
    inline constexpr bool TestIsFinal = TestIsFinalHelper<A>::VALUE;

    template<class A>
    inline constexpr bool TestIsScopedEnum = TestIsScopedEnumHelper<A>::VALUE;


}